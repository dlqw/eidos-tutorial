param(
    [string]$CliProject = "",
    [string]$Root = "",
    [switch]$NoBuild,
    [string]$Phase = "hir"
)

$ErrorActionPreference = "Continue"
$PSNativeCommandUseErrorActionPreference = $false

$ScriptDirectory = Split-Path -Parent $PSCommandPath

if ([string]::IsNullOrWhiteSpace($Root))
{
    $Root = $ScriptDirectory
}

$Root = (Resolve-Path $Root).Path

Push-Location $Root
try
{
    if ([string]::IsNullOrWhiteSpace($CliProject))
    {
        $candidates = @(
            $env:EIDOSC_CLI_PROJECT,
            (Join-Path $Root "Eidosc/src/Eidosc.Cli"),
            (Join-Path $Root "src/Eidosc.Cli"),
            (Join-Path $ScriptDirectory "../Eidosc/src/Eidosc.Cli"),
            (Join-Path $ScriptDirectory "../../Eidosc/src/Eidosc.Cli")
        ) | Where-Object { -not [string]::IsNullOrWhiteSpace($_) }

        $CliProject = $candidates |
            Where-Object { Test-Path $_ } |
            Select-Object -First 1

        if ([string]::IsNullOrWhiteSpace($CliProject))
        {
            Write-Host "FAIL | Eidosc.Cli was not found. Pass -CliProject or set EIDOSC_CLI_PROJECT."
            exit 1
        }

        $CliProject = (Resolve-Path $CliProject).Path
    }

    $examplesRoot = Join-Path $ScriptDirectory "examples"
    if (-not (Test-Path $examplesRoot))
    {
        Write-Host "FAIL | examples root not found: $examplesRoot"
        exit 1
    }

    $phaseOverrides = @{
        "06_nested_call_parser_only.eidos" = "parser"
    }

    $expectedFailures = @{
        "36_hkt_trait_constraint_kind_mismatch.eidos" = "kind"
    }

    $cases = Get-ChildItem $examplesRoot -Filter "*.eidos" |
        Sort-Object Name |
        ForEach-Object {
            $casePhase = if ($phaseOverrides.ContainsKey($_.Name)) { $phaseOverrides[$_.Name] } else { $Phase }
            [pscustomobject]@{
                Name = $_.Name
                File = $_.FullName
                Phase = $casePhase
                ShouldPass = -not $expectedFailures.ContainsKey($_.Name)
                Expect = if ($expectedFailures.ContainsKey($_.Name)) { $expectedFailures[$_.Name] } else { "" }
            }
        }

    $failed = $false
    $results = New-Object System.Collections.Generic.List[string]

    foreach ($case in $cases)
    {
        $displayPath = Resolve-Path -Relative $case.File
        $runArgs = @("run", "--project", $CliProject)
        if ($NoBuild)
        {
            $runArgs += "--no-build"
        }

        $runArgs += @("--", "analyze", $case.File, "--phase", $case.Phase, "--no-color")
        $output = & dotnet @runArgs 2>&1
        $commandExitCode = $LASTEXITCODE
        $joined = ($output | Out-String)

        if ($case.ShouldPass)
        {
            if ($commandExitCode -ne 0)
            {
                $failed = $true
                $results.Add("FAIL | $displayPath | phase=$($case.Phase) | expected pass")
                $results.Add($joined.TrimEnd())
                continue
            }

            $results.Add("PASS | $displayPath | phase=$($case.Phase)")
            continue
        }

        if ($commandExitCode -eq 0)
        {
            $failed = $true
            $results.Add("FAIL | $displayPath | phase=$($case.Phase) | expected failure but passed")
            continue
        }

        if (-not [string]::IsNullOrWhiteSpace($case.Expect) -and -not $joined.Contains($case.Expect))
        {
            $failed = $true
            $results.Add("FAIL | $displayPath | phase=$($case.Phase) | failure text mismatch")
            $results.Add($joined.TrimEnd())
            continue
        }

        $results.Add("PASS | $displayPath | phase=$($case.Phase) | expected failure observed")
    }

    $buildHostProject = Join-Path $examplesRoot "build_host"
    if (Test-Path $buildHostProject)
    {
        $graphOutput = Join-Path ([System.IO.Path]::GetTempPath()) "eidos-build-host-tutorial-$([Guid]::NewGuid().ToString('N')).json"
        try
        {
            $runArgs = @("run", "--project", $CliProject)
            if ($NoBuild)
            {
                $runArgs += "--no-build"
            }

            $runArgs += @("--", "build", "--project", $buildHostProject, "--target", "typed", "--no-cache", "--emit-build-graph", $graphOutput, "--no-color")
            $output = & dotnet @runArgs 2>&1
            if ($LASTEXITCODE -ne 0 -or -not (Test-Path $graphOutput))
            {
                $failed = $true
                $results.Add("FAIL | examples/build_host | expected Build host project to emit a graph")
                $results.Add(($output | Out-String).TrimEnd())
            }
            else
            {
                $results.Add("PASS | examples/build_host | capability-constrained BuildGraph")
            }
        }
        finally
        {
            Remove-Item -LiteralPath $graphOutput -Force -ErrorAction SilentlyContinue
        }
    }

    $results | ForEach-Object { Write-Host $_ }

    if ($failed)
    {
        exit 1
    }

    exit 0
}
finally
{
    Pop-Location
}

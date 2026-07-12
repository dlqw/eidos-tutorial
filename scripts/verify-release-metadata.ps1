param(
    [string]$RepositoryRoot = ""
)

$ErrorActionPreference = "Stop"
if ([string]::IsNullOrWhiteSpace($RepositoryRoot))
{
    $RepositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
}

$languageMetadata = Get-Content -Raw (Join-Path $RepositoryRoot "eidos-language.toml")
$versionMatch = [regex]::Match($languageMetadata, '(?m)^version\s*=\s*"([^"]+)"')
if (-not $versionMatch.Success) { throw "eidos-language.toml does not declare version." }
$version = $versionMatch.Groups[1].Value

$manifest = Get-Content -Raw (Join-Path $RepositoryRoot "eidos.toml")
if ($manifest -notmatch '(?m)^manifestSchema\s*=\s*3\s*$') { throw "Tutorial manifest must use schema 3." }
$manifestVersion = [regex]::Match($manifest, '(?ms)^\[language\].*?^version\s*=\s*"([^"]+)"')
if (-not $manifestVersion.Success -or $manifestVersion.Groups[1].Value -ne $version)
{
    throw "Tutorial manifest language version does not match eidos-language.toml."
}

$changelog = Join-Path $RepositoryRoot "changelogs/$version.md"
if (-not (Test-Path -LiteralPath $changelog)) { throw "Missing language changelog: $changelog" }

Write-Host "Eidos language release metadata verified: $version"

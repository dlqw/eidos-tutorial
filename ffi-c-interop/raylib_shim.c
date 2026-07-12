// Auto-generated C shim for raylib FFI bindings
// Source: refs/raylib/src/raylib.h
// Generated: 2026-05-12 04:59:19 UTC

#include "raylib.h"

void eidos_shim_SetWindowIcon(void *image_ptr) { return SetWindowIcon(*(Image*)image_ptr); }

void* eidos_shim_GetMonitorPosition(int monitor) {
    GetMonitorPosition_result = GetMonitorPosition(monitor);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_GetWindowPosition() {
    GetWindowPosition_result = GetWindowPosition();
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_GetWindowScaleDPI() {
    GetWindowScaleDPI_result = GetWindowScaleDPI();
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_GetClipboardImage() {
    GetClipboardImage_result = GetClipboardImage();
    // TODO: return struct-by-value via pointer
    return NULL;
}

void eidos_shim_ClearBackground(void *color_ptr) { return ClearBackground(*(Color*)color_ptr); }

void eidos_shim_BeginMode2D(void *camera_ptr) { return BeginMode2D(*(Camera2D*)camera_ptr); }

void eidos_shim_BeginMode3D(void *camera_ptr) { return BeginMode3D(*(Camera3D*)camera_ptr); }

void eidos_shim_BeginShaderMode(void *shader_ptr) { return BeginShaderMode(*(Shader*)shader_ptr); }

void eidos_shim_BeginVrStereoMode(void *config_ptr) { return BeginVrStereoMode(*(VrStereoConfig*)config_ptr); }

void* eidos_shim_LoadVrStereoConfig(void *device_ptr) {
    LoadVrStereoConfig_result = LoadVrStereoConfig(*(VrDeviceInfo*)device_ptr);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void eidos_shim_UnloadVrStereoConfig(void *config_ptr) { return UnloadVrStereoConfig(*(VrStereoConfig*)config_ptr); }

void* eidos_shim_LoadShader(const char * vsFileName, const char * fsFileName) {
    LoadShader_result = LoadShader(vsFileName, fsFileName);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_LoadShaderFromMemory(const char * vsCode, const char * fsCode) {
    LoadShaderFromMemory_result = LoadShaderFromMemory(vsCode, fsCode);
    // TODO: return struct-by-value via pointer
    return NULL;
}

_Bool eidos_shim_IsShaderValid(void *shader_ptr) { return IsShaderValid(*(Shader*)shader_ptr); }

int eidos_shim_GetShaderLocation(void *shader_ptr, const char * uniformName) { return GetShaderLocation(*(Shader*)shader_ptr, uniformName); }

int eidos_shim_GetShaderLocationAttrib(void *shader_ptr, const char * attribName) { return GetShaderLocationAttrib(*(Shader*)shader_ptr, attribName); }

void eidos_shim_SetShaderValue(void *shader_ptr, int locIndex, const void * value, int uniformType) { return SetShaderValue(*(Shader*)shader_ptr, locIndex, value, uniformType); }

void eidos_shim_SetShaderValueV(void *shader_ptr, int locIndex, const void * value, int uniformType, int count) { return SetShaderValueV(*(Shader*)shader_ptr, locIndex, value, uniformType, count); }

void eidos_shim_SetShaderValueMatrix(void *shader_ptr, int locIndex, void *mat_ptr) { return SetShaderValueMatrix(*(Shader*)shader_ptr, locIndex, *(Matrix*)mat_ptr); }

void eidos_shim_SetShaderValueTexture(void *shader_ptr, int locIndex, Texture2D texture) { return SetShaderValueTexture(*(Shader*)shader_ptr, locIndex, texture); }

void eidos_shim_UnloadShader(void *shader_ptr) { return UnloadShader(*(Shader*)shader_ptr); }

void* eidos_shim_GetScreenToWorldRay(void *position_ptr, Camera camera) {
    GetScreenToWorldRay_result = GetScreenToWorldRay(*(Vector2*)position_ptr, camera);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_GetScreenToWorldRayEx(void *position_ptr, Camera camera, int width, int height) {
    GetScreenToWorldRayEx_result = GetScreenToWorldRayEx(*(Vector2*)position_ptr, camera, width, height);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_GetWorldToScreen(void *position_ptr, Camera camera) {
    GetWorldToScreen_result = GetWorldToScreen(*(Vector3*)position_ptr, camera);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_GetWorldToScreenEx(void *position_ptr, Camera camera, int width, int height) {
    GetWorldToScreenEx_result = GetWorldToScreenEx(*(Vector3*)position_ptr, camera, width, height);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_GetWorldToScreen2D(void *position_ptr, void *camera_ptr) {
    GetWorldToScreen2D_result = GetWorldToScreen2D(*(Vector2*)position_ptr, *(Camera2D*)camera_ptr);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_GetScreenToWorld2D(void *position_ptr, void *camera_ptr) {
    GetScreenToWorld2D_result = GetScreenToWorld2D(*(Vector2*)position_ptr, *(Camera2D*)camera_ptr);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_GetCameraMatrix(Camera camera) {
    GetCameraMatrix_result = GetCameraMatrix(camera);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_GetCameraMatrix2D(void *camera_ptr) {
    GetCameraMatrix2D_result = GetCameraMatrix2D(*(Camera2D*)camera_ptr);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_LoadDirectoryFiles(const char * dirPath) {
    LoadDirectoryFiles_result = LoadDirectoryFiles(dirPath);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_LoadDirectoryFilesEx(const char * basePath, const char * filter, _Bool scanSubdirs) {
    LoadDirectoryFilesEx_result = LoadDirectoryFilesEx(basePath, filter, scanSubdirs);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void eidos_shim_UnloadDirectoryFiles(void *files_ptr) { return UnloadDirectoryFiles(*(FilePathList*)files_ptr); }

void* eidos_shim_LoadDroppedFiles() {
    LoadDroppedFiles_result = LoadDroppedFiles();
    // TODO: return struct-by-value via pointer
    return NULL;
}

void eidos_shim_UnloadDroppedFiles(void *files_ptr) { return UnloadDroppedFiles(*(FilePathList*)files_ptr); }

void* eidos_shim_LoadAutomationEventList(const char * fileName) {
    LoadAutomationEventList_result = LoadAutomationEventList(fileName);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void eidos_shim_UnloadAutomationEventList(void *list_ptr) { return UnloadAutomationEventList(*(AutomationEventList*)list_ptr); }

_Bool eidos_shim_ExportAutomationEventList(void *list_ptr, const char * fileName) { return ExportAutomationEventList(*(AutomationEventList*)list_ptr, fileName); }

void eidos_shim_PlayAutomationEvent(void *event_ptr) { return PlayAutomationEvent(*(AutomationEvent*)event_ptr); }

void* eidos_shim_GetMousePosition() {
    GetMousePosition_result = GetMousePosition();
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_GetMouseDelta() {
    GetMouseDelta_result = GetMouseDelta();
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_GetMouseWheelMoveV() {
    GetMouseWheelMoveV_result = GetMouseWheelMoveV();
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_GetTouchPosition(int index) {
    GetTouchPosition_result = GetTouchPosition(index);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_GetGestureDragVector() {
    GetGestureDragVector_result = GetGestureDragVector();
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_GetGesturePinchVector() {
    GetGesturePinchVector_result = GetGesturePinchVector();
    // TODO: return struct-by-value via pointer
    return NULL;
}

void eidos_shim_UpdateCameraPro(Camera * camera, void *movement_ptr, void *rotation_ptr, float zoom) { return UpdateCameraPro(camera, *(Vector3*)movement_ptr, *(Vector3*)rotation_ptr, zoom); }

void eidos_shim_SetShapesTexture(Texture2D texture, void *source_ptr) { return SetShapesTexture(texture, *(Rectangle*)source_ptr); }

void* eidos_shim_GetShapesTextureRectangle() {
    GetShapesTextureRectangle_result = GetShapesTextureRectangle();
    // TODO: return struct-by-value via pointer
    return NULL;
}

void eidos_shim_DrawPixel(int posX, int posY, void *color_ptr) { return DrawPixel(posX, posY, *(Color*)color_ptr); }

void eidos_shim_DrawPixelV(void *position_ptr, void *color_ptr) { return DrawPixelV(*(Vector2*)position_ptr, *(Color*)color_ptr); }

void eidos_shim_DrawLine(int startPosX, int startPosY, int endPosX, int endPosY, void *color_ptr) { return DrawLine(startPosX, startPosY, endPosX, endPosY, *(Color*)color_ptr); }

void eidos_shim_DrawLineV(void *startPos_ptr, void *endPos_ptr, void *color_ptr) { return DrawLineV(*(Vector2*)startPos_ptr, *(Vector2*)endPos_ptr, *(Color*)color_ptr); }

void eidos_shim_DrawLineEx(void *startPos_ptr, void *endPos_ptr, float thick, void *color_ptr) { return DrawLineEx(*(Vector2*)startPos_ptr, *(Vector2*)endPos_ptr, thick, *(Color*)color_ptr); }

void eidos_shim_DrawLineStrip(const Vector2 * points, int pointCount, void *color_ptr) { return DrawLineStrip(points, pointCount, *(Color*)color_ptr); }

void eidos_shim_DrawLineBezier(void *startPos_ptr, void *endPos_ptr, float thick, void *color_ptr) { return DrawLineBezier(*(Vector2*)startPos_ptr, *(Vector2*)endPos_ptr, thick, *(Color*)color_ptr); }

void eidos_shim_DrawLineDashed(void *startPos_ptr, void *endPos_ptr, int dashSize, int spaceSize, void *color_ptr) { return DrawLineDashed(*(Vector2*)startPos_ptr, *(Vector2*)endPos_ptr, dashSize, spaceSize, *(Color*)color_ptr); }

void eidos_shim_DrawCircle(int centerX, int centerY, float radius, void *color_ptr) { return DrawCircle(centerX, centerY, radius, *(Color*)color_ptr); }

void eidos_shim_DrawCircleV(void *center_ptr, float radius, void *color_ptr) { return DrawCircleV(*(Vector2*)center_ptr, radius, *(Color*)color_ptr); }

void eidos_shim_DrawCircleGradient(void *center_ptr, float radius, void *inner_ptr, void *outer_ptr) { return DrawCircleGradient(*(Vector2*)center_ptr, radius, *(Color*)inner_ptr, *(Color*)outer_ptr); }

void eidos_shim_DrawCircleSector(void *center_ptr, float radius, float startAngle, float endAngle, int segments, void *color_ptr) { return DrawCircleSector(*(Vector2*)center_ptr, radius, startAngle, endAngle, segments, *(Color*)color_ptr); }

void eidos_shim_DrawCircleSectorLines(void *center_ptr, float radius, float startAngle, float endAngle, int segments, void *color_ptr) { return DrawCircleSectorLines(*(Vector2*)center_ptr, radius, startAngle, endAngle, segments, *(Color*)color_ptr); }

void eidos_shim_DrawCircleLines(int centerX, int centerY, float radius, void *color_ptr) { return DrawCircleLines(centerX, centerY, radius, *(Color*)color_ptr); }

void eidos_shim_DrawCircleLinesV(void *center_ptr, float radius, void *color_ptr) { return DrawCircleLinesV(*(Vector2*)center_ptr, radius, *(Color*)color_ptr); }

void eidos_shim_DrawEllipse(int centerX, int centerY, float radiusH, float radiusV, void *color_ptr) { return DrawEllipse(centerX, centerY, radiusH, radiusV, *(Color*)color_ptr); }

void eidos_shim_DrawEllipseV(void *center_ptr, float radiusH, float radiusV, void *color_ptr) { return DrawEllipseV(*(Vector2*)center_ptr, radiusH, radiusV, *(Color*)color_ptr); }

void eidos_shim_DrawEllipseLines(int centerX, int centerY, float radiusH, float radiusV, void *color_ptr) { return DrawEllipseLines(centerX, centerY, radiusH, radiusV, *(Color*)color_ptr); }

void eidos_shim_DrawEllipseLinesV(void *center_ptr, float radiusH, float radiusV, void *color_ptr) { return DrawEllipseLinesV(*(Vector2*)center_ptr, radiusH, radiusV, *(Color*)color_ptr); }

void eidos_shim_DrawRing(void *center_ptr, float innerRadius, float outerRadius, float startAngle, float endAngle, int segments, void *color_ptr) { return DrawRing(*(Vector2*)center_ptr, innerRadius, outerRadius, startAngle, endAngle, segments, *(Color*)color_ptr); }

void eidos_shim_DrawRingLines(void *center_ptr, float innerRadius, float outerRadius, float startAngle, float endAngle, int segments, void *color_ptr) { return DrawRingLines(*(Vector2*)center_ptr, innerRadius, outerRadius, startAngle, endAngle, segments, *(Color*)color_ptr); }

void eidos_shim_DrawRectangle(int posX, int posY, int width, int height, void *color_ptr) { return DrawRectangle(posX, posY, width, height, *(Color*)color_ptr); }

void eidos_shim_DrawRectangleV(void *position_ptr, void *size_ptr, void *color_ptr) { return DrawRectangleV(*(Vector2*)position_ptr, *(Vector2*)size_ptr, *(Color*)color_ptr); }

void eidos_shim_DrawRectangleRec(void *rec_ptr, void *color_ptr) { return DrawRectangleRec(*(Rectangle*)rec_ptr, *(Color*)color_ptr); }

void eidos_shim_DrawRectanglePro(void *rec_ptr, void *origin_ptr, float rotation, void *color_ptr) { return DrawRectanglePro(*(Rectangle*)rec_ptr, *(Vector2*)origin_ptr, rotation, *(Color*)color_ptr); }

void eidos_shim_DrawRectangleGradientV(int posX, int posY, int width, int height, void *top_ptr, void *bottom_ptr) { return DrawRectangleGradientV(posX, posY, width, height, *(Color*)top_ptr, *(Color*)bottom_ptr); }

void eidos_shim_DrawRectangleGradientH(int posX, int posY, int width, int height, void *left_ptr, void *right_ptr) { return DrawRectangleGradientH(posX, posY, width, height, *(Color*)left_ptr, *(Color*)right_ptr); }

void eidos_shim_DrawRectangleGradientEx(void *rec_ptr, void *topLeft_ptr, void *bottomLeft_ptr, void *bottomRight_ptr, void *topRight_ptr) { return DrawRectangleGradientEx(*(Rectangle*)rec_ptr, *(Color*)topLeft_ptr, *(Color*)bottomLeft_ptr, *(Color*)bottomRight_ptr, *(Color*)topRight_ptr); }

void eidos_shim_DrawRectangleLines(int posX, int posY, int width, int height, void *color_ptr) { return DrawRectangleLines(posX, posY, width, height, *(Color*)color_ptr); }

void eidos_shim_DrawRectangleLinesEx(void *rec_ptr, float lineThick, void *color_ptr) { return DrawRectangleLinesEx(*(Rectangle*)rec_ptr, lineThick, *(Color*)color_ptr); }

void eidos_shim_DrawRectangleRounded(void *rec_ptr, float roundness, int segments, void *color_ptr) { return DrawRectangleRounded(*(Rectangle*)rec_ptr, roundness, segments, *(Color*)color_ptr); }

void eidos_shim_DrawRectangleRoundedLines(void *rec_ptr, float roundness, int segments, void *color_ptr) { return DrawRectangleRoundedLines(*(Rectangle*)rec_ptr, roundness, segments, *(Color*)color_ptr); }

void eidos_shim_DrawRectangleRoundedLinesEx(void *rec_ptr, float roundness, int segments, float lineThick, void *color_ptr) { return DrawRectangleRoundedLinesEx(*(Rectangle*)rec_ptr, roundness, segments, lineThick, *(Color*)color_ptr); }

void eidos_shim_DrawTriangle(void *v1_ptr, void *v2_ptr, void *v3_ptr, void *color_ptr) { return DrawTriangle(*(Vector2*)v1_ptr, *(Vector2*)v2_ptr, *(Vector2*)v3_ptr, *(Color*)color_ptr); }

void eidos_shim_DrawTriangleGradient(void *v1_ptr, void *v2_ptr, void *v3_ptr, void *c1_ptr, void *c2_ptr, void *c3_ptr) { return DrawTriangleGradient(*(Vector2*)v1_ptr, *(Vector2*)v2_ptr, *(Vector2*)v3_ptr, *(Color*)c1_ptr, *(Color*)c2_ptr, *(Color*)c3_ptr); }

void eidos_shim_DrawTriangleLines(void *v1_ptr, void *v2_ptr, void *v3_ptr, void *color_ptr) { return DrawTriangleLines(*(Vector2*)v1_ptr, *(Vector2*)v2_ptr, *(Vector2*)v3_ptr, *(Color*)color_ptr); }

void eidos_shim_DrawTriangleFan(const Vector2 * points, int pointCount, void *color_ptr) { return DrawTriangleFan(points, pointCount, *(Color*)color_ptr); }

void eidos_shim_DrawTriangleStrip(const Vector2 * points, int pointCount, void *color_ptr) { return DrawTriangleStrip(points, pointCount, *(Color*)color_ptr); }

void eidos_shim_DrawPoly(void *center_ptr, int sides, float radius, float rotation, void *color_ptr) { return DrawPoly(*(Vector2*)center_ptr, sides, radius, rotation, *(Color*)color_ptr); }

void eidos_shim_DrawPolyLines(void *center_ptr, int sides, float radius, float rotation, void *color_ptr) { return DrawPolyLines(*(Vector2*)center_ptr, sides, radius, rotation, *(Color*)color_ptr); }

void eidos_shim_DrawPolyLinesEx(void *center_ptr, int sides, float radius, float rotation, float lineThick, void *color_ptr) { return DrawPolyLinesEx(*(Vector2*)center_ptr, sides, radius, rotation, lineThick, *(Color*)color_ptr); }

void eidos_shim_DrawSplineLinear(const Vector2 * points, int pointCount, float thick, void *color_ptr) { return DrawSplineLinear(points, pointCount, thick, *(Color*)color_ptr); }

void eidos_shim_DrawSplineBasis(const Vector2 * points, int pointCount, float thick, void *color_ptr) { return DrawSplineBasis(points, pointCount, thick, *(Color*)color_ptr); }

void eidos_shim_DrawSplineCatmullRom(const Vector2 * points, int pointCount, float thick, void *color_ptr) { return DrawSplineCatmullRom(points, pointCount, thick, *(Color*)color_ptr); }

void eidos_shim_DrawSplineBezierQuadratic(const Vector2 * points, int pointCount, float thick, void *color_ptr) { return DrawSplineBezierQuadratic(points, pointCount, thick, *(Color*)color_ptr); }

void eidos_shim_DrawSplineBezierCubic(const Vector2 * points, int pointCount, float thick, void *color_ptr) { return DrawSplineBezierCubic(points, pointCount, thick, *(Color*)color_ptr); }

void eidos_shim_DrawSplineSegmentLinear(void *p1_ptr, void *p2_ptr, float thick, void *color_ptr) { return DrawSplineSegmentLinear(*(Vector2*)p1_ptr, *(Vector2*)p2_ptr, thick, *(Color*)color_ptr); }

void eidos_shim_DrawSplineSegmentBasis(void *p1_ptr, void *p2_ptr, void *p3_ptr, void *p4_ptr, float thick, void *color_ptr) { return DrawSplineSegmentBasis(*(Vector2*)p1_ptr, *(Vector2*)p2_ptr, *(Vector2*)p3_ptr, *(Vector2*)p4_ptr, thick, *(Color*)color_ptr); }

void eidos_shim_DrawSplineSegmentCatmullRom(void *p1_ptr, void *p2_ptr, void *p3_ptr, void *p4_ptr, float thick, void *color_ptr) { return DrawSplineSegmentCatmullRom(*(Vector2*)p1_ptr, *(Vector2*)p2_ptr, *(Vector2*)p3_ptr, *(Vector2*)p4_ptr, thick, *(Color*)color_ptr); }

void eidos_shim_DrawSplineSegmentBezierQuadratic(void *p1_ptr, void *c2_ptr, void *p3_ptr, float thick, void *color_ptr) { return DrawSplineSegmentBezierQuadratic(*(Vector2*)p1_ptr, *(Vector2*)c2_ptr, *(Vector2*)p3_ptr, thick, *(Color*)color_ptr); }

void eidos_shim_DrawSplineSegmentBezierCubic(void *p1_ptr, void *c2_ptr, void *c3_ptr, void *p4_ptr, float thick, void *color_ptr) { return DrawSplineSegmentBezierCubic(*(Vector2*)p1_ptr, *(Vector2*)c2_ptr, *(Vector2*)c3_ptr, *(Vector2*)p4_ptr, thick, *(Color*)color_ptr); }

void* eidos_shim_GetSplinePointLinear(void *startPos_ptr, void *endPos_ptr, float t) {
    GetSplinePointLinear_result = GetSplinePointLinear(*(Vector2*)startPos_ptr, *(Vector2*)endPos_ptr, t);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_GetSplinePointBasis(void *p1_ptr, void *p2_ptr, void *p3_ptr, void *p4_ptr, float t) {
    GetSplinePointBasis_result = GetSplinePointBasis(*(Vector2*)p1_ptr, *(Vector2*)p2_ptr, *(Vector2*)p3_ptr, *(Vector2*)p4_ptr, t);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_GetSplinePointCatmullRom(void *p1_ptr, void *p2_ptr, void *p3_ptr, void *p4_ptr, float t) {
    GetSplinePointCatmullRom_result = GetSplinePointCatmullRom(*(Vector2*)p1_ptr, *(Vector2*)p2_ptr, *(Vector2*)p3_ptr, *(Vector2*)p4_ptr, t);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_GetSplinePointBezierQuadratic(void *p1_ptr, void *c2_ptr, void *p3_ptr, float t) {
    GetSplinePointBezierQuadratic_result = GetSplinePointBezierQuadratic(*(Vector2*)p1_ptr, *(Vector2*)c2_ptr, *(Vector2*)p3_ptr, t);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_GetSplinePointBezierCubic(void *p1_ptr, void *c2_ptr, void *c3_ptr, void *p4_ptr, float t) {
    GetSplinePointBezierCubic_result = GetSplinePointBezierCubic(*(Vector2*)p1_ptr, *(Vector2*)c2_ptr, *(Vector2*)c3_ptr, *(Vector2*)p4_ptr, t);
    // TODO: return struct-by-value via pointer
    return NULL;
}

_Bool eidos_shim_CheckCollisionRecs(void *rec1_ptr, void *rec2_ptr) { return CheckCollisionRecs(*(Rectangle*)rec1_ptr, *(Rectangle*)rec2_ptr); }

_Bool eidos_shim_CheckCollisionCircles(void *center1_ptr, float radius1, void *center2_ptr, float radius2) { return CheckCollisionCircles(*(Vector2*)center1_ptr, radius1, *(Vector2*)center2_ptr, radius2); }

_Bool eidos_shim_CheckCollisionCircleRec(void *center_ptr, float radius, void *rec_ptr) { return CheckCollisionCircleRec(*(Vector2*)center_ptr, radius, *(Rectangle*)rec_ptr); }

_Bool eidos_shim_CheckCollisionCircleLine(void *center_ptr, float radius, void *p1_ptr, void *p2_ptr) { return CheckCollisionCircleLine(*(Vector2*)center_ptr, radius, *(Vector2*)p1_ptr, *(Vector2*)p2_ptr); }

_Bool eidos_shim_CheckCollisionPointRec(void *point_ptr, void *rec_ptr) { return CheckCollisionPointRec(*(Vector2*)point_ptr, *(Rectangle*)rec_ptr); }

_Bool eidos_shim_CheckCollisionPointCircle(void *point_ptr, void *center_ptr, float radius) { return CheckCollisionPointCircle(*(Vector2*)point_ptr, *(Vector2*)center_ptr, radius); }

_Bool eidos_shim_CheckCollisionPointTriangle(void *point_ptr, void *p1_ptr, void *p2_ptr, void *p3_ptr) { return CheckCollisionPointTriangle(*(Vector2*)point_ptr, *(Vector2*)p1_ptr, *(Vector2*)p2_ptr, *(Vector2*)p3_ptr); }

_Bool eidos_shim_CheckCollisionPointLine(void *point_ptr, void *p1_ptr, void *p2_ptr, int threshold) { return CheckCollisionPointLine(*(Vector2*)point_ptr, *(Vector2*)p1_ptr, *(Vector2*)p2_ptr, threshold); }

_Bool eidos_shim_CheckCollisionPointPoly(void *point_ptr, const Vector2 * points, int pointCount) { return CheckCollisionPointPoly(*(Vector2*)point_ptr, points, pointCount); }

_Bool eidos_shim_CheckCollisionLines(void *startPos1_ptr, void *endPos1_ptr, void *startPos2_ptr, void *endPos2_ptr, Vector2 * collisionPoint) { return CheckCollisionLines(*(Vector2*)startPos1_ptr, *(Vector2*)endPos1_ptr, *(Vector2*)startPos2_ptr, *(Vector2*)endPos2_ptr, collisionPoint); }

void* eidos_shim_GetCollisionRec(void *rec1_ptr, void *rec2_ptr) {
    GetCollisionRec_result = GetCollisionRec(*(Rectangle*)rec1_ptr, *(Rectangle*)rec2_ptr);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_LoadImage(const char * fileName) {
    LoadImage_result = LoadImage(fileName);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_LoadImageRaw(const char * fileName, int width, int height, int format, int headerSize) {
    LoadImageRaw_result = LoadImageRaw(fileName, width, height, format, headerSize);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_LoadImageAnim(const char * fileName, int * frames) {
    LoadImageAnim_result = LoadImageAnim(fileName, frames);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_LoadImageAnimFromMemory(const char * fileType, const unsigned char * fileData, int dataSize, int * frames) {
    LoadImageAnimFromMemory_result = LoadImageAnimFromMemory(fileType, fileData, dataSize, frames);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_LoadImageFromMemory(const char * fileType, const unsigned char * fileData, int dataSize) {
    LoadImageFromMemory_result = LoadImageFromMemory(fileType, fileData, dataSize);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_LoadImageFromTexture(Texture2D texture) {
    LoadImageFromTexture_result = LoadImageFromTexture(texture);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_LoadImageFromScreen() {
    LoadImageFromScreen_result = LoadImageFromScreen();
    // TODO: return struct-by-value via pointer
    return NULL;
}

_Bool eidos_shim_IsImageValid(void *image_ptr) { return IsImageValid(*(Image*)image_ptr); }

void eidos_shim_UnloadImage(void *image_ptr) { return UnloadImage(*(Image*)image_ptr); }

_Bool eidos_shim_ExportImage(void *image_ptr, const char * fileName) { return ExportImage(*(Image*)image_ptr, fileName); }

unsigned char * eidos_shim_ExportImageToMemory(void *image_ptr, const char * fileType, int * fileSize) { return ExportImageToMemory(*(Image*)image_ptr, fileType, fileSize); }

_Bool eidos_shim_ExportImageAsCode(void *image_ptr, const char * fileName) { return ExportImageAsCode(*(Image*)image_ptr, fileName); }

void* eidos_shim_GenImageColor(int width, int height, void *color_ptr) {
    GenImageColor_result = GenImageColor(width, height, *(Color*)color_ptr);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_GenImageGradientLinear(int width, int height, int direction, void *start_ptr, void *end_ptr) {
    GenImageGradientLinear_result = GenImageGradientLinear(width, height, direction, *(Color*)start_ptr, *(Color*)end_ptr);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_GenImageGradientRadial(int width, int height, float density, void *inner_ptr, void *outer_ptr) {
    GenImageGradientRadial_result = GenImageGradientRadial(width, height, density, *(Color*)inner_ptr, *(Color*)outer_ptr);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_GenImageGradientSquare(int width, int height, float density, void *inner_ptr, void *outer_ptr) {
    GenImageGradientSquare_result = GenImageGradientSquare(width, height, density, *(Color*)inner_ptr, *(Color*)outer_ptr);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_GenImageChecked(int width, int height, int checksX, int checksY, void *col1_ptr, void *col2_ptr) {
    GenImageChecked_result = GenImageChecked(width, height, checksX, checksY, *(Color*)col1_ptr, *(Color*)col2_ptr);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_GenImageWhiteNoise(int width, int height, float factor) {
    GenImageWhiteNoise_result = GenImageWhiteNoise(width, height, factor);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_GenImagePerlinNoise(int width, int height, int offsetX, int offsetY, float scale) {
    GenImagePerlinNoise_result = GenImagePerlinNoise(width, height, offsetX, offsetY, scale);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_GenImageCellular(int width, int height, int tileSize) {
    GenImageCellular_result = GenImageCellular(width, height, tileSize);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_GenImageText(int width, int height, const char * text) {
    GenImageText_result = GenImageText(width, height, text);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_ImageCopy(void *image_ptr) {
    ImageCopy_result = ImageCopy(*(Image*)image_ptr);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_ImageFromImage(void *image_ptr, void *rec_ptr) {
    ImageFromImage_result = ImageFromImage(*(Image*)image_ptr, *(Rectangle*)rec_ptr);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_ImageFromChannel(void *image_ptr, int selectedChannel) {
    ImageFromChannel_result = ImageFromChannel(*(Image*)image_ptr, selectedChannel);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_ImageText(const char * text, int fontSize, void *color_ptr) {
    ImageText_result = ImageText(text, fontSize, *(Color*)color_ptr);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_ImageTextEx(void *font_ptr, const char * text, float fontSize, float spacing, void *tint_ptr) {
    ImageTextEx_result = ImageTextEx(*(Font*)font_ptr, text, fontSize, spacing, *(Color*)tint_ptr);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void eidos_shim_ImageToPOT(Image * image, void *fill_ptr) { return ImageToPOT(image, *(Color*)fill_ptr); }

void eidos_shim_ImageCrop(Image * image, void *crop_ptr) { return ImageCrop(image, *(Rectangle*)crop_ptr); }

void eidos_shim_ImageAlphaClear(Image * image, void *color_ptr, float threshold) { return ImageAlphaClear(image, *(Color*)color_ptr, threshold); }

void eidos_shim_ImageAlphaMask(Image * image, void *alphaMask_ptr) { return ImageAlphaMask(image, *(Image*)alphaMask_ptr); }

void eidos_shim_ImageResizeCanvas(Image * image, int newWidth, int newHeight, int offsetX, int offsetY, void *fill_ptr) { return ImageResizeCanvas(image, newWidth, newHeight, offsetX, offsetY, *(Color*)fill_ptr); }

void eidos_shim_ImageColorTint(Image * image, void *color_ptr) { return ImageColorTint(image, *(Color*)color_ptr); }

void eidos_shim_ImageColorReplace(Image * image, void *color_ptr, void *replace_ptr) { return ImageColorReplace(image, *(Color*)color_ptr, *(Color*)replace_ptr); }

Color * eidos_shim_LoadImageColors(void *image_ptr) { return LoadImageColors(*(Image*)image_ptr); }

Color * eidos_shim_LoadImagePalette(void *image_ptr, int maxPaletteSize, int * colorCount) { return LoadImagePalette(*(Image*)image_ptr, maxPaletteSize, colorCount); }

void* eidos_shim_GetImageAlphaBorder(void *image_ptr, float threshold) {
    GetImageAlphaBorder_result = GetImageAlphaBorder(*(Image*)image_ptr, threshold);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_GetImageColor(void *image_ptr, int x, int y) {
    GetImageColor_result = GetImageColor(*(Image*)image_ptr, x, y);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void eidos_shim_ImageClearBackground(Image * dst, void *color_ptr) { return ImageClearBackground(dst, *(Color*)color_ptr); }

void eidos_shim_ImageDrawPixel(Image * dst, int posX, int posY, void *color_ptr) { return ImageDrawPixel(dst, posX, posY, *(Color*)color_ptr); }

void eidos_shim_ImageDrawPixelV(Image * dst, void *position_ptr, void *color_ptr) { return ImageDrawPixelV(dst, *(Vector2*)position_ptr, *(Color*)color_ptr); }

void eidos_shim_ImageDrawLine(Image * dst, int startPosX, int startPosY, int endPosX, int endPosY, void *color_ptr) { return ImageDrawLine(dst, startPosX, startPosY, endPosX, endPosY, *(Color*)color_ptr); }

void eidos_shim_ImageDrawLineV(Image * dst, void *start_ptr, void *end_ptr, void *color_ptr) { return ImageDrawLineV(dst, *(Vector2*)start_ptr, *(Vector2*)end_ptr, *(Color*)color_ptr); }

void eidos_shim_ImageDrawLineEx(Image * dst, void *start_ptr, void *end_ptr, int thick, void *color_ptr) { return ImageDrawLineEx(dst, *(Vector2*)start_ptr, *(Vector2*)end_ptr, thick, *(Color*)color_ptr); }

void eidos_shim_ImageDrawCircle(Image * dst, int centerX, int centerY, int radius, void *color_ptr) { return ImageDrawCircle(dst, centerX, centerY, radius, *(Color*)color_ptr); }

void eidos_shim_ImageDrawCircleV(Image * dst, void *center_ptr, int radius, void *color_ptr) { return ImageDrawCircleV(dst, *(Vector2*)center_ptr, radius, *(Color*)color_ptr); }

void eidos_shim_ImageDrawCircleLines(Image * dst, int centerX, int centerY, int radius, void *color_ptr) { return ImageDrawCircleLines(dst, centerX, centerY, radius, *(Color*)color_ptr); }

void eidos_shim_ImageDrawCircleLinesV(Image * dst, void *center_ptr, int radius, void *color_ptr) { return ImageDrawCircleLinesV(dst, *(Vector2*)center_ptr, radius, *(Color*)color_ptr); }

void eidos_shim_ImageDrawRectangle(Image * dst, int posX, int posY, int width, int height, void *color_ptr) { return ImageDrawRectangle(dst, posX, posY, width, height, *(Color*)color_ptr); }

void eidos_shim_ImageDrawRectangleV(Image * dst, void *position_ptr, void *size_ptr, void *color_ptr) { return ImageDrawRectangleV(dst, *(Vector2*)position_ptr, *(Vector2*)size_ptr, *(Color*)color_ptr); }

void eidos_shim_ImageDrawRectangleRec(Image * dst, void *rec_ptr, void *color_ptr) { return ImageDrawRectangleRec(dst, *(Rectangle*)rec_ptr, *(Color*)color_ptr); }

void eidos_shim_ImageDrawRectangleLines(Image * dst, int posX, int posY, int width, int height, void *color_ptr) { return ImageDrawRectangleLines(dst, posX, posY, width, height, *(Color*)color_ptr); }

void eidos_shim_ImageDrawRectangleLinesEx(Image * dst, void *rec_ptr, int thick, void *color_ptr) { return ImageDrawRectangleLinesEx(dst, *(Rectangle*)rec_ptr, thick, *(Color*)color_ptr); }

void eidos_shim_ImageDrawTriangle(Image * dst, void *v1_ptr, void *v2_ptr, void *v3_ptr, void *color_ptr) { return ImageDrawTriangle(dst, *(Vector2*)v1_ptr, *(Vector2*)v2_ptr, *(Vector2*)v3_ptr, *(Color*)color_ptr); }

void eidos_shim_ImageDrawTriangleGradient(Image * dst, void *v1_ptr, void *v2_ptr, void *v3_ptr, void *c1_ptr, void *c2_ptr, void *c3_ptr) { return ImageDrawTriangleGradient(dst, *(Vector2*)v1_ptr, *(Vector2*)v2_ptr, *(Vector2*)v3_ptr, *(Color*)c1_ptr, *(Color*)c2_ptr, *(Color*)c3_ptr); }

void eidos_shim_ImageDrawTriangleLines(Image * dst, void *v1_ptr, void *v2_ptr, void *v3_ptr, void *color_ptr) { return ImageDrawTriangleLines(dst, *(Vector2*)v1_ptr, *(Vector2*)v2_ptr, *(Vector2*)v3_ptr, *(Color*)color_ptr); }

void eidos_shim_ImageDrawTriangleFan(Image * dst, const Vector2 * points, int pointCount, void *color_ptr) { return ImageDrawTriangleFan(dst, points, pointCount, *(Color*)color_ptr); }

void eidos_shim_ImageDrawTriangleStrip(Image * dst, const Vector2 * points, int pointCount, void *color_ptr) { return ImageDrawTriangleStrip(dst, points, pointCount, *(Color*)color_ptr); }

void eidos_shim_ImageDraw(Image * dst, void *src_ptr, void *srcRec_ptr, void *dstRec_ptr, void *tint_ptr) { return ImageDraw(dst, *(Image*)src_ptr, *(Rectangle*)srcRec_ptr, *(Rectangle*)dstRec_ptr, *(Color*)tint_ptr); }

void eidos_shim_ImageDrawText(Image * dst, const char * text, int posX, int posY, int fontSize, void *color_ptr) { return ImageDrawText(dst, text, posX, posY, fontSize, *(Color*)color_ptr); }

void eidos_shim_ImageDrawTextEx(Image * dst, void *font_ptr, const char * text, void *position_ptr, float fontSize, float spacing, void *tint_ptr) { return ImageDrawTextEx(dst, *(Font*)font_ptr, text, *(Vector2*)position_ptr, fontSize, spacing, *(Color*)tint_ptr); }

Texture2D eidos_shim_LoadTextureFromImage(void *image_ptr) { return LoadTextureFromImage(*(Image*)image_ptr); }

TextureCubemap eidos_shim_LoadTextureCubemap(void *image_ptr, int layout) { return LoadTextureCubemap(*(Image*)image_ptr, layout); }

void eidos_shim_UpdateTextureRec(Texture2D texture, void *rec_ptr, const void * pixels) { return UpdateTextureRec(texture, *(Rectangle*)rec_ptr, pixels); }

void eidos_shim_DrawTexture(Texture2D texture, int posX, int posY, void *tint_ptr) { return DrawTexture(texture, posX, posY, *(Color*)tint_ptr); }

void eidos_shim_DrawTextureV(Texture2D texture, void *position_ptr, void *tint_ptr) { return DrawTextureV(texture, *(Vector2*)position_ptr, *(Color*)tint_ptr); }

void eidos_shim_DrawTextureEx(Texture2D texture, void *position_ptr, float rotation, float scale, void *tint_ptr) { return DrawTextureEx(texture, *(Vector2*)position_ptr, rotation, scale, *(Color*)tint_ptr); }

void eidos_shim_DrawTextureRec(Texture2D texture, void *source_ptr, void *position_ptr, void *tint_ptr) { return DrawTextureRec(texture, *(Rectangle*)source_ptr, *(Vector2*)position_ptr, *(Color*)tint_ptr); }

void eidos_shim_DrawTexturePro(Texture2D texture, void *source_ptr, void *dest_ptr, void *origin_ptr, float rotation, void *tint_ptr) { return DrawTexturePro(texture, *(Rectangle*)source_ptr, *(Rectangle*)dest_ptr, *(Vector2*)origin_ptr, rotation, *(Color*)tint_ptr); }

void eidos_shim_DrawTextureNPatch(Texture2D texture, void *nPatchInfo_ptr, void *dest_ptr, void *origin_ptr, float rotation, void *tint_ptr) { return DrawTextureNPatch(texture, *(NPatchInfo*)nPatchInfo_ptr, *(Rectangle*)dest_ptr, *(Vector2*)origin_ptr, rotation, *(Color*)tint_ptr); }

_Bool eidos_shim_ColorIsEqual(void *col1_ptr, void *col2_ptr) { return ColorIsEqual(*(Color*)col1_ptr, *(Color*)col2_ptr); }

void* eidos_shim_Fade(void *color_ptr, float alpha) {
    Fade_result = Fade(*(Color*)color_ptr, alpha);
    // TODO: return struct-by-value via pointer
    return NULL;
}

int eidos_shim_ColorToInt(void *color_ptr) { return ColorToInt(*(Color*)color_ptr); }

void* eidos_shim_ColorNormalize(void *color_ptr) {
    ColorNormalize_result = ColorNormalize(*(Color*)color_ptr);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_ColorFromNormalized(void *normalized_ptr) {
    ColorFromNormalized_result = ColorFromNormalized(*(Vector4*)normalized_ptr);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_ColorToHSV(void *color_ptr) {
    ColorToHSV_result = ColorToHSV(*(Color*)color_ptr);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_ColorFromHSV(float hue, float saturation, float value) {
    ColorFromHSV_result = ColorFromHSV(hue, saturation, value);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_ColorTint(void *color_ptr, void *tint_ptr) {
    ColorTint_result = ColorTint(*(Color*)color_ptr, *(Color*)tint_ptr);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_ColorBrightness(void *color_ptr, float factor) {
    ColorBrightness_result = ColorBrightness(*(Color*)color_ptr, factor);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_ColorContrast(void *color_ptr, float contrast) {
    ColorContrast_result = ColorContrast(*(Color*)color_ptr, contrast);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_ColorAlpha(void *color_ptr, float alpha) {
    ColorAlpha_result = ColorAlpha(*(Color*)color_ptr, alpha);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_ColorAlphaBlend(void *dst_ptr, void *src_ptr, void *tint_ptr) {
    ColorAlphaBlend_result = ColorAlphaBlend(*(Color*)dst_ptr, *(Color*)src_ptr, *(Color*)tint_ptr);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_ColorLerp(void *color1_ptr, void *color2_ptr, float factor) {
    ColorLerp_result = ColorLerp(*(Color*)color1_ptr, *(Color*)color2_ptr, factor);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_GetColor(unsigned int hexValue) {
    GetColor_result = GetColor(hexValue);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_GetPixelColor(void * srcPtr, int format) {
    GetPixelColor_result = GetPixelColor(srcPtr, format);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void eidos_shim_SetPixelColor(void * dstPtr, void *color_ptr, int format) { return SetPixelColor(dstPtr, *(Color*)color_ptr, format); }

void* eidos_shim_GetFontDefault() {
    GetFontDefault_result = GetFontDefault();
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_LoadFont(const char * fileName) {
    LoadFont_result = LoadFont(fileName);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_LoadFontEx(const char * fileName, int fontSize, const int * codepoints, int codepointCount) {
    LoadFontEx_result = LoadFontEx(fileName, fontSize, codepoints, codepointCount);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_LoadFontFromImage(void *image_ptr, void *key_ptr, int firstChar) {
    LoadFontFromImage_result = LoadFontFromImage(*(Image*)image_ptr, *(Color*)key_ptr, firstChar);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_LoadFontFromMemory(const char * fileType, const unsigned char * fileData, int dataSize, int fontSize, const int * codepoints, int codepointCount) {
    LoadFontFromMemory_result = LoadFontFromMemory(fileType, fileData, dataSize, fontSize, codepoints, codepointCount);
    // TODO: return struct-by-value via pointer
    return NULL;
}

_Bool eidos_shim_IsFontValid(void *font_ptr) { return IsFontValid(*(Font*)font_ptr); }

void* eidos_shim_GenImageFontAtlas(const GlyphInfo * glyphs, Rectangle ** glyphRecs, int glyphCount, int fontSize, int padding, int packMethod) {
    GenImageFontAtlas_result = GenImageFontAtlas(glyphs, glyphRecs, glyphCount, fontSize, padding, packMethod);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void eidos_shim_UnloadFont(void *font_ptr) { return UnloadFont(*(Font*)font_ptr); }

_Bool eidos_shim_ExportFontAsCode(void *font_ptr, const char * fileName) { return ExportFontAsCode(*(Font*)font_ptr, fileName); }

void eidos_shim_DrawText(const char * text, int posX, int posY, int fontSize, void *color_ptr) { return DrawText(text, posX, posY, fontSize, *(Color*)color_ptr); }

void eidos_shim_DrawTextEx(void *font_ptr, const char * text, void *position_ptr, float fontSize, float spacing, void *tint_ptr) { return DrawTextEx(*(Font*)font_ptr, text, *(Vector2*)position_ptr, fontSize, spacing, *(Color*)tint_ptr); }

void eidos_shim_DrawTextPro(void *font_ptr, const char * text, void *position_ptr, void *origin_ptr, float rotation, float fontSize, float spacing, void *tint_ptr) { return DrawTextPro(*(Font*)font_ptr, text, *(Vector2*)position_ptr, *(Vector2*)origin_ptr, rotation, fontSize, spacing, *(Color*)tint_ptr); }

void eidos_shim_DrawTextCodepoint(void *font_ptr, int codepoint, void *position_ptr, float fontSize, void *tint_ptr) { return DrawTextCodepoint(*(Font*)font_ptr, codepoint, *(Vector2*)position_ptr, fontSize, *(Color*)tint_ptr); }

void eidos_shim_DrawTextCodepoints(void *font_ptr, const int * codepoints, int codepointCount, void *position_ptr, float fontSize, float spacing, void *tint_ptr) { return DrawTextCodepoints(*(Font*)font_ptr, codepoints, codepointCount, *(Vector2*)position_ptr, fontSize, spacing, *(Color*)tint_ptr); }

void* eidos_shim_MeasureTextEx(void *font_ptr, const char * text, float fontSize, float spacing) {
    MeasureTextEx_result = MeasureTextEx(*(Font*)font_ptr, text, fontSize, spacing);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_MeasureTextCodepoints(void *font_ptr, const int * codepoints, int length, float fontSize, float spacing) {
    MeasureTextCodepoints_result = MeasureTextCodepoints(*(Font*)font_ptr, codepoints, length, fontSize, spacing);
    // TODO: return struct-by-value via pointer
    return NULL;
}

int eidos_shim_GetGlyphIndex(void *font_ptr, int codepoint) { return GetGlyphIndex(*(Font*)font_ptr, codepoint); }

void* eidos_shim_GetGlyphInfo(void *font_ptr, int codepoint) {
    GetGlyphInfo_result = GetGlyphInfo(*(Font*)font_ptr, codepoint);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_GetGlyphAtlasRec(void *font_ptr, int codepoint) {
    GetGlyphAtlasRec_result = GetGlyphAtlasRec(*(Font*)font_ptr, codepoint);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void eidos_shim_DrawLine3D(void *startPos_ptr, void *endPos_ptr, void *color_ptr) { return DrawLine3D(*(Vector3*)startPos_ptr, *(Vector3*)endPos_ptr, *(Color*)color_ptr); }

void eidos_shim_DrawPoint3D(void *position_ptr, void *color_ptr) { return DrawPoint3D(*(Vector3*)position_ptr, *(Color*)color_ptr); }

void eidos_shim_DrawCircle3D(void *center_ptr, float radius, void *rotationAxis_ptr, float rotationAngle, void *color_ptr) { return DrawCircle3D(*(Vector3*)center_ptr, radius, *(Vector3*)rotationAxis_ptr, rotationAngle, *(Color*)color_ptr); }

void eidos_shim_DrawTriangle3D(void *v1_ptr, void *v2_ptr, void *v3_ptr, void *color_ptr) { return DrawTriangle3D(*(Vector3*)v1_ptr, *(Vector3*)v2_ptr, *(Vector3*)v3_ptr, *(Color*)color_ptr); }

void eidos_shim_DrawTriangleStrip3D(const Vector3 * points, int pointCount, void *color_ptr) { return DrawTriangleStrip3D(points, pointCount, *(Color*)color_ptr); }

void eidos_shim_DrawCube(void *position_ptr, float width, float height, float length, void *color_ptr) { return DrawCube(*(Vector3*)position_ptr, width, height, length, *(Color*)color_ptr); }

void eidos_shim_DrawCubeV(void *position_ptr, void *size_ptr, void *color_ptr) { return DrawCubeV(*(Vector3*)position_ptr, *(Vector3*)size_ptr, *(Color*)color_ptr); }

void eidos_shim_DrawCubeWires(void *position_ptr, float width, float height, float length, void *color_ptr) { return DrawCubeWires(*(Vector3*)position_ptr, width, height, length, *(Color*)color_ptr); }

void eidos_shim_DrawCubeWiresV(void *position_ptr, void *size_ptr, void *color_ptr) { return DrawCubeWiresV(*(Vector3*)position_ptr, *(Vector3*)size_ptr, *(Color*)color_ptr); }

void eidos_shim_DrawSphere(void *centerPos_ptr, float radius, void *color_ptr) { return DrawSphere(*(Vector3*)centerPos_ptr, radius, *(Color*)color_ptr); }

void eidos_shim_DrawSphereEx(void *centerPos_ptr, float radius, int rings, int slices, void *color_ptr) { return DrawSphereEx(*(Vector3*)centerPos_ptr, radius, rings, slices, *(Color*)color_ptr); }

void eidos_shim_DrawSphereWires(void *centerPos_ptr, float radius, int rings, int slices, void *color_ptr) { return DrawSphereWires(*(Vector3*)centerPos_ptr, radius, rings, slices, *(Color*)color_ptr); }

void eidos_shim_DrawCylinder(void *position_ptr, float radiusTop, float radiusBottom, float height, int slices, void *color_ptr) { return DrawCylinder(*(Vector3*)position_ptr, radiusTop, radiusBottom, height, slices, *(Color*)color_ptr); }

void eidos_shim_DrawCylinderEx(void *startPos_ptr, void *endPos_ptr, float startRadius, float endRadius, int sides, void *color_ptr) { return DrawCylinderEx(*(Vector3*)startPos_ptr, *(Vector3*)endPos_ptr, startRadius, endRadius, sides, *(Color*)color_ptr); }

void eidos_shim_DrawCylinderWires(void *position_ptr, float radiusTop, float radiusBottom, float height, int slices, void *color_ptr) { return DrawCylinderWires(*(Vector3*)position_ptr, radiusTop, radiusBottom, height, slices, *(Color*)color_ptr); }

void eidos_shim_DrawCylinderWiresEx(void *startPos_ptr, void *endPos_ptr, float startRadius, float endRadius, int slices, void *color_ptr) { return DrawCylinderWiresEx(*(Vector3*)startPos_ptr, *(Vector3*)endPos_ptr, startRadius, endRadius, slices, *(Color*)color_ptr); }

void eidos_shim_DrawCapsule(void *startPos_ptr, void *endPos_ptr, float radius, int rings, int slices, void *color_ptr) { return DrawCapsule(*(Vector3*)startPos_ptr, *(Vector3*)endPos_ptr, radius, rings, slices, *(Color*)color_ptr); }

void eidos_shim_DrawCapsuleWires(void *startPos_ptr, void *endPos_ptr, float radius, int rings, int slices, void *color_ptr) { return DrawCapsuleWires(*(Vector3*)startPos_ptr, *(Vector3*)endPos_ptr, radius, rings, slices, *(Color*)color_ptr); }

void eidos_shim_DrawPlane(void *centerPos_ptr, void *size_ptr, void *color_ptr) { return DrawPlane(*(Vector3*)centerPos_ptr, *(Vector2*)size_ptr, *(Color*)color_ptr); }

void eidos_shim_DrawRay(void *ray_ptr, void *color_ptr) { return DrawRay(*(Ray*)ray_ptr, *(Color*)color_ptr); }

void* eidos_shim_LoadModel(const char * fileName) {
    LoadModel_result = LoadModel(fileName);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_LoadModelFromMesh(void *mesh_ptr) {
    LoadModelFromMesh_result = LoadModelFromMesh(*(Mesh*)mesh_ptr);
    // TODO: return struct-by-value via pointer
    return NULL;
}

_Bool eidos_shim_IsModelValid(void *model_ptr) { return IsModelValid(*(Model*)model_ptr); }

void eidos_shim_UnloadModel(void *model_ptr) { return UnloadModel(*(Model*)model_ptr); }

void* eidos_shim_GetModelBoundingBox(void *model_ptr) {
    GetModelBoundingBox_result = GetModelBoundingBox(*(Model*)model_ptr);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void eidos_shim_DrawModel(void *model_ptr, void *position_ptr, float scale, void *tint_ptr) { return DrawModel(*(Model*)model_ptr, *(Vector3*)position_ptr, scale, *(Color*)tint_ptr); }

void eidos_shim_DrawModelEx(void *model_ptr, void *position_ptr, void *rotationAxis_ptr, float rotationAngle, void *scale_ptr, void *tint_ptr) { return DrawModelEx(*(Model*)model_ptr, *(Vector3*)position_ptr, *(Vector3*)rotationAxis_ptr, rotationAngle, *(Vector3*)scale_ptr, *(Color*)tint_ptr); }

void eidos_shim_DrawModelWires(void *model_ptr, void *position_ptr, float scale, void *tint_ptr) { return DrawModelWires(*(Model*)model_ptr, *(Vector3*)position_ptr, scale, *(Color*)tint_ptr); }

void eidos_shim_DrawModelWiresEx(void *model_ptr, void *position_ptr, void *rotationAxis_ptr, float rotationAngle, void *scale_ptr, void *tint_ptr) { return DrawModelWiresEx(*(Model*)model_ptr, *(Vector3*)position_ptr, *(Vector3*)rotationAxis_ptr, rotationAngle, *(Vector3*)scale_ptr, *(Color*)tint_ptr); }

void eidos_shim_DrawBoundingBox(void *box_ptr, void *color_ptr) { return DrawBoundingBox(*(BoundingBox*)box_ptr, *(Color*)color_ptr); }

void eidos_shim_DrawBillboard(Camera camera, Texture2D texture, void *position_ptr, float scale, void *tint_ptr) { return DrawBillboard(camera, texture, *(Vector3*)position_ptr, scale, *(Color*)tint_ptr); }

void eidos_shim_DrawBillboardRec(Camera camera, Texture2D texture, void *source_ptr, void *position_ptr, void *size_ptr, void *tint_ptr) { return DrawBillboardRec(camera, texture, *(Rectangle*)source_ptr, *(Vector3*)position_ptr, *(Vector2*)size_ptr, *(Color*)tint_ptr); }

void eidos_shim_DrawBillboardPro(Camera camera, Texture2D texture, void *source_ptr, void *position_ptr, void *up_ptr, void *size_ptr, void *origin_ptr, float rotation, void *tint_ptr) { return DrawBillboardPro(camera, texture, *(Rectangle*)source_ptr, *(Vector3*)position_ptr, *(Vector3*)up_ptr, *(Vector2*)size_ptr, *(Vector2*)origin_ptr, rotation, *(Color*)tint_ptr); }

void eidos_shim_UpdateMeshBuffer(void *mesh_ptr, int index, const void * data, int dataSize, int offset) { return UpdateMeshBuffer(*(Mesh*)mesh_ptr, index, data, dataSize, offset); }

void eidos_shim_UnloadMesh(void *mesh_ptr) { return UnloadMesh(*(Mesh*)mesh_ptr); }

void eidos_shim_DrawMesh(void *mesh_ptr, void *material_ptr, void *transform_ptr) { return DrawMesh(*(Mesh*)mesh_ptr, *(Material*)material_ptr, *(Matrix*)transform_ptr); }

void eidos_shim_DrawMeshInstanced(void *mesh_ptr, void *material_ptr, const Matrix * transforms, int instances) { return DrawMeshInstanced(*(Mesh*)mesh_ptr, *(Material*)material_ptr, transforms, instances); }

void* eidos_shim_GetMeshBoundingBox(void *mesh_ptr) {
    GetMeshBoundingBox_result = GetMeshBoundingBox(*(Mesh*)mesh_ptr);
    // TODO: return struct-by-value via pointer
    return NULL;
}

_Bool eidos_shim_ExportMesh(void *mesh_ptr, const char * fileName) { return ExportMesh(*(Mesh*)mesh_ptr, fileName); }

_Bool eidos_shim_ExportMeshAsCode(void *mesh_ptr, const char * fileName) { return ExportMeshAsCode(*(Mesh*)mesh_ptr, fileName); }

void* eidos_shim_GenMeshPoly(int sides, float radius) {
    GenMeshPoly_result = GenMeshPoly(sides, radius);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_GenMeshPlane(float width, float length, int resX, int resZ) {
    GenMeshPlane_result = GenMeshPlane(width, length, resX, resZ);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_GenMeshCube(float width, float height, float length) {
    GenMeshCube_result = GenMeshCube(width, height, length);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_GenMeshSphere(float radius, int rings, int slices) {
    GenMeshSphere_result = GenMeshSphere(radius, rings, slices);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_GenMeshHemiSphere(float radius, int rings, int slices) {
    GenMeshHemiSphere_result = GenMeshHemiSphere(radius, rings, slices);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_GenMeshCylinder(float radius, float height, int slices) {
    GenMeshCylinder_result = GenMeshCylinder(radius, height, slices);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_GenMeshCone(float radius, float height, int slices) {
    GenMeshCone_result = GenMeshCone(radius, height, slices);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_GenMeshTorus(float radius, float size, int radSeg, int sides) {
    GenMeshTorus_result = GenMeshTorus(radius, size, radSeg, sides);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_GenMeshKnot(float radius, float size, int radSeg, int sides) {
    GenMeshKnot_result = GenMeshKnot(radius, size, radSeg, sides);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_GenMeshHeightmap(void *heightmap_ptr, void *size_ptr) {
    GenMeshHeightmap_result = GenMeshHeightmap(*(Image*)heightmap_ptr, *(Vector3*)size_ptr);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_GenMeshCubicmap(void *cubicmap_ptr, void *cubeSize_ptr) {
    GenMeshCubicmap_result = GenMeshCubicmap(*(Image*)cubicmap_ptr, *(Vector3*)cubeSize_ptr);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_LoadMaterialDefault() {
    LoadMaterialDefault_result = LoadMaterialDefault();
    // TODO: return struct-by-value via pointer
    return NULL;
}

_Bool eidos_shim_IsMaterialValid(void *material_ptr) { return IsMaterialValid(*(Material*)material_ptr); }

void eidos_shim_UnloadMaterial(void *material_ptr) { return UnloadMaterial(*(Material*)material_ptr); }

void eidos_shim_UpdateModelAnimation(void *model_ptr, void *anim_ptr, float frame) { return UpdateModelAnimation(*(Model*)model_ptr, *(ModelAnimation*)anim_ptr, frame); }

void eidos_shim_UpdateModelAnimationEx(void *model_ptr, void *animA_ptr, float frameA, void *animB_ptr, float frameB, float blend) { return UpdateModelAnimationEx(*(Model*)model_ptr, *(ModelAnimation*)animA_ptr, frameA, *(ModelAnimation*)animB_ptr, frameB, blend); }

_Bool eidos_shim_IsModelAnimationValid(void *model_ptr, void *anim_ptr) { return IsModelAnimationValid(*(Model*)model_ptr, *(ModelAnimation*)anim_ptr); }

_Bool eidos_shim_CheckCollisionSpheres(void *center1_ptr, float radius1, void *center2_ptr, float radius2) { return CheckCollisionSpheres(*(Vector3*)center1_ptr, radius1, *(Vector3*)center2_ptr, radius2); }

_Bool eidos_shim_CheckCollisionBoxes(void *box1_ptr, void *box2_ptr) { return CheckCollisionBoxes(*(BoundingBox*)box1_ptr, *(BoundingBox*)box2_ptr); }

_Bool eidos_shim_CheckCollisionBoxSphere(void *box_ptr, void *center_ptr, float radius) { return CheckCollisionBoxSphere(*(BoundingBox*)box_ptr, *(Vector3*)center_ptr, radius); }

void* eidos_shim_GetRayCollisionSphere(void *ray_ptr, void *center_ptr, float radius) {
    GetRayCollisionSphere_result = GetRayCollisionSphere(*(Ray*)ray_ptr, *(Vector3*)center_ptr, radius);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_GetRayCollisionBox(void *ray_ptr, void *box_ptr) {
    GetRayCollisionBox_result = GetRayCollisionBox(*(Ray*)ray_ptr, *(BoundingBox*)box_ptr);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_GetRayCollisionMesh(void *ray_ptr, void *mesh_ptr, void *transform_ptr) {
    GetRayCollisionMesh_result = GetRayCollisionMesh(*(Ray*)ray_ptr, *(Mesh*)mesh_ptr, *(Matrix*)transform_ptr);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_GetRayCollisionTriangle(void *ray_ptr, void *p1_ptr, void *p2_ptr, void *p3_ptr) {
    GetRayCollisionTriangle_result = GetRayCollisionTriangle(*(Ray*)ray_ptr, *(Vector3*)p1_ptr, *(Vector3*)p2_ptr, *(Vector3*)p3_ptr);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_GetRayCollisionQuad(void *ray_ptr, void *p1_ptr, void *p2_ptr, void *p3_ptr, void *p4_ptr) {
    GetRayCollisionQuad_result = GetRayCollisionQuad(*(Ray*)ray_ptr, *(Vector3*)p1_ptr, *(Vector3*)p2_ptr, *(Vector3*)p3_ptr, *(Vector3*)p4_ptr);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_LoadWave(const char * fileName) {
    LoadWave_result = LoadWave(fileName);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_LoadWaveFromMemory(const char * fileType, const unsigned char * fileData, int dataSize) {
    LoadWaveFromMemory_result = LoadWaveFromMemory(fileType, fileData, dataSize);
    // TODO: return struct-by-value via pointer
    return NULL;
}

_Bool eidos_shim_IsWaveValid(void *wave_ptr) { return IsWaveValid(*(Wave*)wave_ptr); }

void* eidos_shim_LoadSound(const char * fileName) {
    LoadSound_result = LoadSound(fileName);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_LoadSoundFromWave(void *wave_ptr) {
    LoadSoundFromWave_result = LoadSoundFromWave(*(Wave*)wave_ptr);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_LoadSoundAlias(void *source_ptr) {
    LoadSoundAlias_result = LoadSoundAlias(*(Sound*)source_ptr);
    // TODO: return struct-by-value via pointer
    return NULL;
}

_Bool eidos_shim_IsSoundValid(void *sound_ptr) { return IsSoundValid(*(Sound*)sound_ptr); }

void eidos_shim_UpdateSound(void *sound_ptr, const void * data, int frameCount) { return UpdateSound(*(Sound*)sound_ptr, data, frameCount); }

void eidos_shim_UnloadWave(void *wave_ptr) { return UnloadWave(*(Wave*)wave_ptr); }

void eidos_shim_UnloadSound(void *sound_ptr) { return UnloadSound(*(Sound*)sound_ptr); }

void eidos_shim_UnloadSoundAlias(void *alias_ptr) { return UnloadSoundAlias(*(Sound*)alias_ptr); }

_Bool eidos_shim_ExportWave(void *wave_ptr, const char * fileName) { return ExportWave(*(Wave*)wave_ptr, fileName); }

_Bool eidos_shim_ExportWaveAsCode(void *wave_ptr, const char * fileName) { return ExportWaveAsCode(*(Wave*)wave_ptr, fileName); }

void eidos_shim_PlaySound(void *sound_ptr) { return PlaySound(*(Sound*)sound_ptr); }

void eidos_shim_StopSound(void *sound_ptr) { return StopSound(*(Sound*)sound_ptr); }

void eidos_shim_PauseSound(void *sound_ptr) { return PauseSound(*(Sound*)sound_ptr); }

void eidos_shim_ResumeSound(void *sound_ptr) { return ResumeSound(*(Sound*)sound_ptr); }

_Bool eidos_shim_IsSoundPlaying(void *sound_ptr) { return IsSoundPlaying(*(Sound*)sound_ptr); }

void eidos_shim_SetSoundVolume(void *sound_ptr, float volume) { return SetSoundVolume(*(Sound*)sound_ptr, volume); }

void eidos_shim_SetSoundPitch(void *sound_ptr, float pitch) { return SetSoundPitch(*(Sound*)sound_ptr, pitch); }

void eidos_shim_SetSoundPan(void *sound_ptr, float pan) { return SetSoundPan(*(Sound*)sound_ptr, pan); }

void* eidos_shim_WaveCopy(void *wave_ptr) {
    WaveCopy_result = WaveCopy(*(Wave*)wave_ptr);
    // TODO: return struct-by-value via pointer
    return NULL;
}

float * eidos_shim_LoadWaveSamples(void *wave_ptr) { return LoadWaveSamples(*(Wave*)wave_ptr); }

void* eidos_shim_LoadMusicStream(const char * fileName) {
    LoadMusicStream_result = LoadMusicStream(fileName);
    // TODO: return struct-by-value via pointer
    return NULL;
}

void* eidos_shim_LoadMusicStreamFromMemory(const char * fileType, const unsigned char * data, int dataSize) {
    LoadMusicStreamFromMemory_result = LoadMusicStreamFromMemory(fileType, data, dataSize);
    // TODO: return struct-by-value via pointer
    return NULL;
}

_Bool eidos_shim_IsMusicValid(void *music_ptr) { return IsMusicValid(*(Music*)music_ptr); }

void eidos_shim_UnloadMusicStream(void *music_ptr) { return UnloadMusicStream(*(Music*)music_ptr); }

void eidos_shim_PlayMusicStream(void *music_ptr) { return PlayMusicStream(*(Music*)music_ptr); }

_Bool eidos_shim_IsMusicStreamPlaying(void *music_ptr) { return IsMusicStreamPlaying(*(Music*)music_ptr); }

void eidos_shim_UpdateMusicStream(void *music_ptr) { return UpdateMusicStream(*(Music*)music_ptr); }

void eidos_shim_StopMusicStream(void *music_ptr) { return StopMusicStream(*(Music*)music_ptr); }

void eidos_shim_PauseMusicStream(void *music_ptr) { return PauseMusicStream(*(Music*)music_ptr); }

void eidos_shim_ResumeMusicStream(void *music_ptr) { return ResumeMusicStream(*(Music*)music_ptr); }

void eidos_shim_SeekMusicStream(void *music_ptr, float position) { return SeekMusicStream(*(Music*)music_ptr, position); }

void eidos_shim_SetMusicVolume(void *music_ptr, float volume) { return SetMusicVolume(*(Music*)music_ptr, volume); }

void eidos_shim_SetMusicPitch(void *music_ptr, float pitch) { return SetMusicPitch(*(Music*)music_ptr, pitch); }

void eidos_shim_SetMusicPan(void *music_ptr, float pan) { return SetMusicPan(*(Music*)music_ptr, pan); }

float eidos_shim_GetMusicTimeLength(void *music_ptr) { return GetMusicTimeLength(*(Music*)music_ptr); }

float eidos_shim_GetMusicTimePlayed(void *music_ptr) { return GetMusicTimePlayed(*(Music*)music_ptr); }

void* eidos_shim_LoadAudioStream(unsigned int sampleRate, unsigned int sampleSize, unsigned int channels) {
    LoadAudioStream_result = LoadAudioStream(sampleRate, sampleSize, channels);
    // TODO: return struct-by-value via pointer
    return NULL;
}

_Bool eidos_shim_IsAudioStreamValid(void *stream_ptr) { return IsAudioStreamValid(*(AudioStream*)stream_ptr); }

void eidos_shim_UnloadAudioStream(void *stream_ptr) { return UnloadAudioStream(*(AudioStream*)stream_ptr); }

void eidos_shim_UpdateAudioStream(void *stream_ptr, const void * data, int frameCount) { return UpdateAudioStream(*(AudioStream*)stream_ptr, data, frameCount); }

_Bool eidos_shim_IsAudioStreamProcessed(void *stream_ptr) { return IsAudioStreamProcessed(*(AudioStream*)stream_ptr); }

void eidos_shim_PlayAudioStream(void *stream_ptr) { return PlayAudioStream(*(AudioStream*)stream_ptr); }

void eidos_shim_PauseAudioStream(void *stream_ptr) { return PauseAudioStream(*(AudioStream*)stream_ptr); }

void eidos_shim_ResumeAudioStream(void *stream_ptr) { return ResumeAudioStream(*(AudioStream*)stream_ptr); }

_Bool eidos_shim_IsAudioStreamPlaying(void *stream_ptr) { return IsAudioStreamPlaying(*(AudioStream*)stream_ptr); }

void eidos_shim_StopAudioStream(void *stream_ptr) { return StopAudioStream(*(AudioStream*)stream_ptr); }

void eidos_shim_SetAudioStreamVolume(void *stream_ptr, float volume) { return SetAudioStreamVolume(*(AudioStream*)stream_ptr, volume); }

void eidos_shim_SetAudioStreamPitch(void *stream_ptr, float pitch) { return SetAudioStreamPitch(*(AudioStream*)stream_ptr, pitch); }

void eidos_shim_SetAudioStreamPan(void *stream_ptr, float pan) { return SetAudioStreamPan(*(AudioStream*)stream_ptr, pan); }

void eidos_shim_SetAudioStreamCallback(void *stream_ptr, AudioCallback callback) { return SetAudioStreamCallback(*(AudioStream*)stream_ptr, callback); }

void eidos_shim_AttachAudioStreamProcessor(void *stream_ptr, AudioCallback processor) { return AttachAudioStreamProcessor(*(AudioStream*)stream_ptr, processor); }

void eidos_shim_DetachAudioStreamProcessor(void *stream_ptr, AudioCallback processor) { return DetachAudioStreamProcessor(*(AudioStream*)stream_ptr, processor); }


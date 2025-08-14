#pragma once

#include <cstdint>

class VPANEL;
class IntRect;
class IHTML;
class IHTMLEvents;
class Vertex_t;
class CharRenderInfo;
class IVguiMatInfo;
class IImage;

struct Color
{
	uint8_t r, g, b, a;
};

using InitReturnVal_t	= int;
using FontDrawType_t	= int;
using SurfaceFeature_e	= int;
using ImageFormat		= int;
using HTexture			= unsigned long;
using HFont				= unsigned long;
using HCursor			= unsigned long;

using CreateInterface = void* (const char* name, int* returnCode);


class IAppSystem
{
public:
	virtual bool Connect(CreateInterface factory) = 0;
	virtual void Disconnect() = 0;
	virtual void* QueryInterface(const char* pInterfaceName) = 0;
	virtual InitReturnVal_t Init() = 0;
	virtual void Shutdown() = 0;
};

class ISurface : public IAppSystem
{
	virtual void unkn01() = 0;
	virtual void unkn02() = 0;
	virtual void unkn03() = 0;
	virtual void unkn04() = 0;
public:
	virtual void Shutdown() = 0;
	virtual void RunFrame() = 0;
	virtual VPANEL GetEmbeddedPanel() = 0;
	virtual void SetEmbeddedPanel(VPANEL pPanel) = 0;
	virtual void PushMakeCurrent(VPANEL panel, bool useInsets) = 0;
	virtual void PopMakeCurrent(VPANEL panel) = 0;
	virtual void DrawSetColor(int r, int g, int b, int a) = 0;
	virtual void DrawSetColor(Color col) = 0;
	virtual void DrawFilledRect(int x0, int y0, int x1, int y1) = 0;
	virtual void DrawFilledRectArray(IntRect* pRects, int numRects) = 0;
	virtual void DrawOutlinedRect(int x0, int y0, int x1, int y1) = 0;
	virtual void DrawLine(int x0, int y0, int x1, int y1) = 0;
	virtual void DrawPolyLine(int* px, int* py, int numPoints) = 0;
	virtual void DrawSetTextFont(HFont font) = 0;
	virtual void DrawSetTextColor(int r, int g, int b, int a) = 0;
	virtual void DrawSetTextColor(Color col) = 0;
	virtual void DrawSetTextPos(int x, int y) = 0;
	virtual void DrawGetTextPos(int& x, int& y) = 0;
	virtual void DrawPrintText(const wchar_t* text, int textLen, FontDrawType_t drawType = 0) = 0;
	virtual void DrawUnicodeChar(wchar_t wch, FontDrawType_t drawType = 0) = 0;
	virtual void DrawFlushText() = 0;
	virtual IHTML* CreateHTMLWindow(IHTMLEvents* events, VPANEL context) = 0;
	virtual void PaintHTMLWindow(IHTML* htmlwin) = 0;
	virtual void DeleteHTMLWindow(IHTML* htmlwin) = 0;
	virtual int	 DrawGetTextureId(char const* filename) = 0;
	virtual bool DrawGetTextureFile(int id, char* filename, int maxlen) = 0;
	virtual void DrawSetTextureFile(int id, const char* filename, int hardwareFilter, bool forceReload) = 0;
	virtual void DrawSetTextureRGBA(int id, const unsigned char* rgba, int wide, int tall, int hardwareFilter, bool forceReload) = 0;
	virtual void DrawSetTexture(int id) = 0;
	virtual void DrawGetTextureSize(int id, int& wide, int& tall) = 0;
	virtual void DrawTexturedRect(int x0, int y0, int x1, int y1) = 0;
	virtual bool IsTextureIDValid(int id) = 0;
	virtual int CreateNewTextureID(bool procedural = false) = 0;
	virtual void GetScreenSize(int& wide, int& tall) = 0;
	virtual void SetAsTopMost(VPANEL panel, bool state) = 0;
	virtual void BringToFront(VPANEL panel) = 0;
	virtual void SetForegroundWindow(VPANEL panel) = 0;
	virtual void SetPanelVisible(VPANEL panel, bool state) = 0;
	virtual void SetMinimized(VPANEL panel, bool state) = 0;
	virtual bool IsMinimized(VPANEL panel) = 0;
	virtual void FlashWindow(VPANEL panel, bool state) = 0;
	virtual void SetTitle(VPANEL panel, const wchar_t* title) = 0;
	virtual void SetAsToolBar(VPANEL panel, bool state) = 0;
	virtual void CreatePopup(VPANEL panel, bool minimised, bool showTaskbarIcon = true, bool disabled = false, bool mouseInput = true, bool kbInput = true) = 0;
	virtual void SwapBuffers(VPANEL panel) = 0;
	virtual void Invalidate(VPANEL panel) = 0;
	virtual void SetCursor(HCursor cursor) = 0;
	virtual bool IsCursorVisible() = 0;
	virtual void ApplyChanges() = 0;
	virtual bool IsWithin(int x, int y) = 0;
	virtual bool HasFocus() = 0;
	virtual bool SupportsFeature(SurfaceFeature_e feature) = 0;
	virtual void RestrictPaintToSinglePanel(VPANEL panel) = 0;
	virtual void SetModalPanel(VPANEL) = 0;
	virtual VPANEL GetModalPanel() = 0;
	virtual void UnlockCursor() = 0;
	virtual void LockCursor() = 0;
	virtual void SetTranslateExtendedKeys(bool state) = 0;
	virtual VPANEL GetTopmostPopup() = 0;
	virtual void SetTopLevelFocus(VPANEL panel) = 0;
	virtual HFont CreateFont() = 0;
	virtual bool SetFontGlyphSet(HFont font, const char* windowsFontName, int tall, int weight, int blur, int scanlines, int flags) = 0;
	virtual bool AddCustomFontFile(const char* fontName, const char* fontFileName) = 0;
	virtual int GetFontTall(HFont font) = 0;
	virtual int GetFontAscent(HFont font, wchar_t wch) = 0;
	virtual bool IsFontAdditive(HFont font) = 0;
	virtual void GetCharABCwide(HFont font, int ch, int& a, int& b, int& c) = 0;
	virtual int GetCharacterWidth(HFont font, int ch) = 0;
	virtual void GetTextSize(HFont font, const wchar_t* text, int& wide, int& tall) = 0;
	virtual VPANEL GetNotifyPanel() = 0;
	virtual void SetNotifyIcon(VPANEL context, HTexture icon, VPANEL panelToReceiveMessages, const char* text) = 0;
	virtual void PlaySound(const char* fileName) = 0;
	virtual int GetPopupCount() = 0;
	virtual VPANEL GetPopup(int index) = 0;
	virtual bool ShouldPaintChildPanel(VPANEL childPanel) = 0;
	virtual bool RecreateContext(VPANEL panel) = 0;
	virtual void AddPanel(VPANEL panel) = 0;
	virtual void ReleasePanel(VPANEL panel) = 0;
	virtual void MovePopupToFront(VPANEL panel) = 0;
	virtual void MovePopupToBack(VPANEL panel) = 0;
	virtual void SolveTraverse(VPANEL panel, bool forceApplySchemeSettings = false) = 0;
	virtual void PaintTraverse(VPANEL panel) = 0;
	virtual void EnableMouseCapture(VPANEL panel, bool state) = 0;
	virtual void GetWorkspaceBounds(int& x, int& y, int& wide, int& tall) = 0;
	virtual void GetAbsoluteWindowBounds(int& x, int& y, int& wide, int& tall) = 0;
	virtual void GetProportionalBase(int& width, int& height) = 0;
	virtual void CalculateMouseVisible() = 0;
	virtual bool NeedKBInput() = 0;
	virtual bool HasCursorPosFunctions() = 0;
	virtual void SurfaceGetCursorPos(int& x, int& y) = 0;
	virtual void SurfaceSetCursorPos(int x, int y) = 0;
	virtual void DrawTexturedLine(const Vertex_t& a, const Vertex_t& b) = 0;
	virtual void DrawOutlinedCircle(int x, int y, int radius, int segments) = 0;
	virtual void DrawTexturedPolyLine(const Vertex_t* p, int n) = 0;
	virtual void DrawTexturedSubRect(int x0, int y0, int x1, int y1, float texs0, float text0, float texs1, float text1) = 0;
	virtual void DrawTexturedPolygon(int n, Vertex_t* pVertices) = 0;
	virtual const wchar_t* GetTitle(VPANEL panel) = 0;
	virtual bool IsCursorLocked(void) const = 0;
	virtual void SetWorkspaceInsets(int left, int top, int right, int bottom) = 0;
	virtual bool DrawGetUnicodeCharRenderInfo(wchar_t ch, CharRenderInfo& info) = 0;
	virtual void DrawRenderCharFromInfo(const CharRenderInfo& info) = 0;
	virtual void DrawSetAlphaMultiplier(float alpha) = 0;
	virtual float DrawGetAlphaMultiplier() = 0;
	virtual void SetAllowHTMLJavaScript(bool state) = 0;
	virtual void OnScreenSizeChanged(int nOldWidth, int nOldHeight) = 0;
	virtual IVguiMatInfo* DrawGetTextureMatInfoFactory(int id) = 0;
	virtual void PaintTraverseEx(VPANEL panel, bool paintPopups = false) = 0;
	virtual float GetZPos() const = 0;
	virtual void SetPanelForInput(VPANEL vpanel) = 0;
	virtual void DrawFilledRectFade(int x0, int y0, int x1, int y1, unsigned int alpha0, unsigned int alpha1, bool bHorizontal) = 0;
	virtual void DrawSetTextureRGBAEx(int id, const unsigned char* rgba, int wide, int tall, ImageFormat imageFormat) = 0;
	virtual void DrawSetTextScale(float sx, float sy) = 0;
	virtual bool SetBitmapFontGlyphSet(HFont font, const char* windowsFontName, float scalex, float scaley, int flags) = 0;
	virtual bool AddBitmapFontFile(const char* fontFileName) = 0;
	virtual void SetBitmapFontName(const char* pName, const char* pFontFilename) = 0;
	virtual const char* GetBitmapFontName(const char* pName) = 0;
	virtual IImage* GetIconImageForFullPath(char const* pFullPath) = 0;
	virtual void DrawUnicodeString(const wchar_t* pwString, FontDrawType_t drawType = 0) = 0;

	void StartDrawing();
	void FinishDrawing();
};
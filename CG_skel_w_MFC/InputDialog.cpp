#include "stdafx.h"

#include "CG_skel_w_MFC.h"
#include "InputDialog.h"

#define IDC_CMD_EDIT 200
#define IDC_X_EDIT 201
#define IDC_Y_EDIT 202
#define IDC_Z_EDIT 203

#define CMD_EDIT_TITLE "Command"
#define X_EDIT_TITLE "X ="
#define Y_EDIT_TITLE "Y ="
#define Z_EDIT_TITLE "Z ="

// ------------------------
//    Class CInputDialog
// ------------------------

IMPLEMENT_DYNAMIC(CInputDialog, CDialog)

CInputDialog::CInputDialog(CString title)
	: CDialog(CInputDialog::IDD, NULL), mTitle(title)
{ }

CInputDialog::~CInputDialog()
{ }

BOOL CInputDialog::OnInitDialog()
{
    CDialog::OnInitDialog();

    SetWindowText(mTitle);

    return TRUE;
}

void CInputDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}

// ----------------------
//    Class CCmdDialog
// ----------------------

CCmdDialog::CCmdDialog(CString title)
    : CInputDialog(title), mCmd("")
{ }

CCmdDialog::~CCmdDialog()
{ }

string CCmdDialog::GetCmd()
{
    return CT2CA(mCmd);
}

void CCmdDialog::DoDataExchange(CDataExchange* pDX)
{
    CInputDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_CMD_EDIT, mCmd);
}

// CCmdDialog message handlers
BEGIN_MESSAGE_MAP(CCmdDialog, CInputDialog)
    ON_WM_CREATE ()
    ON_WM_PAINT()
END_MESSAGE_MAP()

int CCmdDialog::OnCreate(LPCREATESTRUCT lpcs)
{
    mCmdEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
      CRect(10, 30, 450, 100), this, IDC_CMD_EDIT);

    return 0;
}

void CCmdDialog::OnPaint()
{   
    CPaintDC dc(this);
    dc.SetBkMode(TRANSPARENT);

    CRect cmd_rect(10, 10, 450, 30);
    dc.DrawText(CString(CMD_EDIT_TITLE), -1, &cmd_rect, DT_SINGLELINE);

    mCmdEdit.SetFocus();
}

// ----------------------
//    Class CXyzDialog
// ----------------------

CXyzDialog::CXyzDialog(CString title, float _mX, float _mY, float _mZ)
    : CInputDialog(title), mX(_mX), mY(_mY), mZ(_mZ), textX("X"), textY("Y"), textZ("Z")
{ }

void CXyzDialog::setText(std::string _textX, std::string _textY, std::string _textZ)
{
    textX = _textX;
    textY = _textY;
    textZ = _textZ;
}

CXyzDialog::~CXyzDialog()
{ }

vec3 CXyzDialog::GetXYZ()
{
    return vec3(mX, mY, mZ);
}

void CXyzDialog::DoDataExchange(CDataExchange* pDX)
{
    CInputDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_X_EDIT, mX);
    DDX_Text(pDX, IDC_Y_EDIT, mY);
    DDX_Text(pDX, IDC_Z_EDIT, mZ);
}

// CXyzDialog message handlers
BEGIN_MESSAGE_MAP(CXyzDialog, CInputDialog)
    ON_WM_CREATE ()
    ON_WM_PAINT()
END_MESSAGE_MAP()

int CXyzDialog::OnCreate(LPCREATESTRUCT lpcs)
{
    mXEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
      CRect(200, 70, 340, 90), this, IDC_X_EDIT);

    mYEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
      CRect(200, 140, 340, 160), this, IDC_Y_EDIT);

    mZEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
      CRect(200, 210, 340, 230), this, IDC_Z_EDIT);

    return 0;
}

void CXyzDialog::OnPaint()
{   
    CPaintDC dc(this);
    dc.SetBkMode(TRANSPARENT);

    CRect x_rect(100, 72, 450, 90);
    dc.DrawText(CString(textX.c_str()), -1, &x_rect, DT_SINGLELINE);

    CRect y_rect(100, 142, 450, 160);
    dc.DrawText(CString(textY.c_str()), -1, &y_rect, DT_SINGLELINE);

    CRect z_rect(100, 212, 450, 230);
    dc.DrawText(CString(textZ.c_str()), -1, &z_rect, DT_SINGLELINE);

    mXEdit.SetFocus();
}

// -------------------------
//    Class CCmdXyzDialog
// -------------------------

CCmdXyzDialog::CCmdXyzDialog(CString title)
    :  CInputDialog(title), mCmd(""), mX(0.0), mY(0.0), mZ(0.0)
{ }

CCmdXyzDialog::~CCmdXyzDialog()
{ }

string CCmdXyzDialog::GetCmd()
{
    return CT2CA(mCmd);
}

vec3 CCmdXyzDialog::GetXYZ()
{
    return vec3(mX, mY, mZ);
}

void CCmdXyzDialog::DoDataExchange(CDataExchange* pDX)
{
    CInputDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_CMD_EDIT, mCmd);
    DDX_Text(pDX, IDC_X_EDIT, mX);
    DDX_Text(pDX, IDC_Y_EDIT, mY);
    DDX_Text(pDX, IDC_Z_EDIT, mZ);
}

// CCmdXyzDialog message handlers
BEGIN_MESSAGE_MAP(CCmdXyzDialog, CInputDialog)
    ON_WM_CREATE ()
    ON_WM_PAINT()
END_MESSAGE_MAP()

int CCmdXyzDialog::OnCreate(LPCREATESTRUCT lpcs)
{    
    mCmdEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
      CRect(10, 30, 450, 100), this, IDC_CMD_EDIT);

    mXEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
      CRect(40, 135, 250, 155), this, IDC_X_EDIT);

    mYEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
      CRect(40, 190, 250, 210), this, IDC_Y_EDIT);

    mZEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
      CRect(40, 245, 250, 265), this, IDC_Z_EDIT);

    return 0;
}

void CCmdXyzDialog::OnPaint()
{   
    CPaintDC dc(this);
    dc.SetBkMode(TRANSPARENT);

    CRect cmd_rect(10, 10, 450, 30);
    dc.DrawText(CString(CMD_EDIT_TITLE), -1, &cmd_rect, DT_SINGLELINE);

    CRect x_rect(10, 137, 450, 155);
    dc.DrawText(CString(X_EDIT_TITLE), -1, &x_rect, DT_SINGLELINE);

    CRect y_rect(10, 192, 450, 210);
    dc.DrawText(CString(Y_EDIT_TITLE), -1, &y_rect, DT_SINGLELINE);

    CRect z_rect(10, 247, 450, 265);
    dc.DrawText(CString(Z_EDIT_TITLE), -1, &z_rect, DT_SINGLELINE);

    mCmdEdit.SetFocus();
}

// -------------------------
//    Class CProjectionDialog
// -------------------------
// time for some custom dialogs pog
// most of this is just copypaste stuff... the only stuff that matters
// is the enum, and the rectangles that define where the text+textboxes go.
// honestly most of the headache is just figuring out the rectangles... 

enum {
    BOTTOM_EDIT = 210, // don't know if there's a reason to start here, i'm just roughly following the convention of the defines above
    LEFT_EDIT,
    ZNEAR_EDIT,
    TOP_EDIT,
    RIGHT_EDIT,
    ZFAR_EDIT
};

void CProjectionDialog::DoDataExchange(CDataExchange* pDX)
{
    // no idea how any of this works, but apparently this automatically verifies that the input is a float,
    // and also makes it so the fields get their value when the dialog closes, so that's cool
    CInputDialog::DoDataExchange(pDX);
    DDX_Text(pDX, BOTTOM_EDIT, bottom);
    DDX_Text(pDX, LEFT_EDIT, left);
    DDX_Text(pDX, ZNEAR_EDIT, zNear);
    DDX_Text(pDX, TOP_EDIT, top);
    DDX_Text(pDX, RIGHT_EDIT, right);
    DDX_Text(pDX, ZFAR_EDIT, zFar);
}

// CProjectionDialog message handlers
BEGIN_MESSAGE_MAP(CProjectionDialog, CInputDialog)
    ON_WM_CREATE()
    ON_WM_PAINT()
END_MESSAGE_MAP()

int CProjectionDialog::OnCreate(LPCREATESTRUCT lpcs)
{ // this creates the textboxes
    bottomEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        CRect(65, 10, 200, 30), this, BOTTOM_EDIT);

    leftEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        CRect(65, 50, 200, 70), this, LEFT_EDIT);

    zNearEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        CRect(65, 90, 200, 110), this, ZNEAR_EDIT);

    topEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        CRect(265, 10, 400, 30), this, TOP_EDIT);

    rightEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        CRect(265, 50, 400, 70), this, RIGHT_EDIT);

    zFarEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        CRect(265, 90, 400, 110), this, ZFAR_EDIT);

    return 0;
}

void CProjectionDialog::OnPaint()
{ // this creates extra text around the textboxes. this extra text is always rendered below the textboxes.
    CPaintDC dc(this);
    dc.SetBkMode(TRANSPARENT);

    CRect bottomRect(10, 10, 200, 30);
    dc.DrawText(CString("Bottom:"), -1, &bottomRect, DT_SINGLELINE);

    CRect leftRect(10, 50, 200, 70);
    dc.DrawText(CString("Left:"), -1, &leftRect, DT_SINGLELINE);

    CRect zNearRec(10, 90, 200, 110);
    dc.DrawText(CString("Near:"), -1, &zNearRec, DT_SINGLELINE);

    CRect topRect(220, 10, 400, 30);
    dc.DrawText(CString("Top:"), -1, &topRect, DT_SINGLELINE);

    CRect rightRect(220, 50, 400, 70);
    dc.DrawText(CString("Right:"), -1, &rightRect, DT_SINGLELINE);

    CRect zFarRec(220, 90, 400, 110);
    dc.DrawText(CString("Far:"), -1, &zFarRec, DT_SINGLELINE);

    bottomEdit.SetFocus();
}

// -------------------------
//    Class CPerspectiveProjectionDialog
// -------------------------

enum {
    PER_ASPECT_EDIT = 210,
    PER_FOVY_EDIT,
    PER_ZNEAR_EDIT,
    PER_ZFAR_EDIT
};

void CPrespectiveProjectionDialog::DoDataExchange(CDataExchange* pDX)
{
    CInputDialog::DoDataExchange(pDX);
    DDX_Text(pDX, PER_ASPECT_EDIT, aspect);
    DDX_Text(pDX, PER_FOVY_EDIT, fovy);
    DDX_Text(pDX, PER_ZNEAR_EDIT, zNear);
    DDX_Text(pDX, PER_ZFAR_EDIT, zFar);
}

// CPrespectiveProjectionDialog message handlers
BEGIN_MESSAGE_MAP(CPrespectiveProjectionDialog, CInputDialog)
    ON_WM_CREATE()
    ON_WM_PAINT()
END_MESSAGE_MAP()

int CPrespectiveProjectionDialog::OnCreate(LPCREATESTRUCT lpcs)
{
    fovyEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        CRect(165, 10, 300, 30), this, PER_FOVY_EDIT);

    aspectEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        CRect(165, 50, 300, 70), this, PER_ASPECT_EDIT);

    zNearEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        CRect(65, 90, 200, 110), this, PER_ZNEAR_EDIT);

    zFarEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        CRect(265, 90, 400, 110), this, PER_ZFAR_EDIT);

    return 0;
}

void CPrespectiveProjectionDialog::OnPaint()
{
    CPaintDC dc(this);
    dc.SetBkMode(TRANSPARENT);

    CRect bottomRect(110, 10, 300, 30);
    dc.DrawText(CString("fov:"), -1, &bottomRect, DT_SINGLELINE);

    CRect leftRect(110, 50, 300, 70);
    dc.DrawText(CString("aspect:"), -1, &leftRect, DT_SINGLELINE);

    CRect zNearRec(10, 90, 200, 110);
    dc.DrawText(CString("Near:"), -1, &zNearRec, DT_SINGLELINE);

    CRect zFarRec(220, 90, 400, 110);
    dc.DrawText(CString("Far:"), -1, &zFarRec, DT_SINGLELINE);

    fovyEdit.SetFocus();
}

// -------------------------
//    Class CSingleFloatDialog
// -------------------------

void CSingleFloatDialog::DoDataExchange(CDataExchange* pDX)
{
    CInputDialog::DoDataExchange(pDX);
    if (isInt) DDX_Text(pDX, IDC_CMD_EDIT, iValue);
    else DDX_Text(pDX, IDC_CMD_EDIT, fValue);
}

// CSingleFloatDialog message handlers
BEGIN_MESSAGE_MAP(CSingleFloatDialog, CInputDialog)
    ON_WM_CREATE()
    ON_WM_PAINT()
END_MESSAGE_MAP()

int CSingleFloatDialog::OnCreate(LPCREATESTRUCT lpcs)
{
    valueEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        CRect(65, 10, 200, 30), this, IDC_CMD_EDIT);

    return 0;
}

void CSingleFloatDialog::OnPaint()
{
    CPaintDC dc(this);
    dc.SetBkMode(TRANSPARENT);

    valueEdit.SetFocus();
}

// -------------------------
//    Class CUniformMaterialDialog
// -------------------------

enum {
    DIFFUSE_RED_EDIT = 210,
    DIFFUSE_GREEN_EDIT,
    DIFFUSE_BLUE_EDIT,
    SPECULAR_RED_EDIT,
    SPECULAR_GREEN_EDIT,
    SPECULAR_BLUE_EDIT,
    EMISSIVE_RED_EDIT,
    EMISSIVE_GREEN_EDIT,
    EMISSIVE_BLUE_EDIT,
    AMBIENT_RED_EDIT,
    AMBIENT_GREEN_EDIT,
    AMBIENT_BLUE_EDIT,
    ROUGHNESS_EDIT,
    SHININESS_EDIT
};

void CUniformMaterialDialog::DoDataExchange(CDataExchange* pDX)
{
    CInputDialog::DoDataExchange(pDX);
    DDX_Text(pDX, DIFFUSE_RED_EDIT, diffuseRed);
    DDX_Text(pDX, DIFFUSE_GREEN_EDIT, diffuseGreen);
    DDX_Text(pDX, DIFFUSE_BLUE_EDIT, diffuseBlue);
    DDX_Text(pDX, SPECULAR_RED_EDIT, specularRed);
    DDX_Text(pDX, SPECULAR_GREEN_EDIT, specularGreen);
    DDX_Text(pDX, SPECULAR_BLUE_EDIT, specularBlue);
    DDX_Text(pDX, EMISSIVE_RED_EDIT, emissiveRed);
    DDX_Text(pDX, EMISSIVE_GREEN_EDIT, emissiveGreen);
    DDX_Text(pDX, EMISSIVE_BLUE_EDIT, emissiveBlue);
    DDX_Text(pDX, AMBIENT_RED_EDIT, ambientRed);
    DDX_Text(pDX, AMBIENT_GREEN_EDIT, ambientGreen);
    DDX_Text(pDX, AMBIENT_BLUE_EDIT, ambientBlue);
    DDX_Text(pDX, ROUGHNESS_EDIT, roughness);
    DDX_Text(pDX, SHININESS_EDIT, shininess);
}

// CUniformMaterialDialog message handlers
BEGIN_MESSAGE_MAP(CUniformMaterialDialog, CInputDialog)
    ON_WM_CREATE()
    ON_WM_PAINT()
END_MESSAGE_MAP()

int CUniformMaterialDialog::OnCreate(LPCREATESTRUCT lpcs)
{
    diffuseRedEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        CRect(135, 10, 200, 30), this, DIFFUSE_RED_EDIT);

    diffuseGreenEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        CRect(135, 50, 200, 70), this, DIFFUSE_GREEN_EDIT);

    diffuseBlueEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        CRect(135, 90, 200, 110), this, DIFFUSE_BLUE_EDIT);

    specularRedEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        CRect(335, 10, 400, 30), this, SPECULAR_RED_EDIT);

    specularGreenEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        CRect(335, 50, 400, 70), this, SPECULAR_GREEN_EDIT);

    specularBlueEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        CRect(335, 90, 400, 110), this, SPECULAR_BLUE_EDIT);

    emissiveRedEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        CRect(135, 130, 200, 150), this, EMISSIVE_RED_EDIT);

    emissiveGreenEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        CRect(135, 170, 200, 190), this, EMISSIVE_GREEN_EDIT);

    emissiveBlueEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        CRect(135, 210, 200, 230), this, EMISSIVE_BLUE_EDIT);

    ambientRedEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        CRect(335, 130, 400, 150), this, AMBIENT_RED_EDIT);

    ambientGreenEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        CRect(335, 170, 400, 190), this, AMBIENT_GREEN_EDIT);

    ambientBlueEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        CRect(335, 210, 400, 230), this, AMBIENT_BLUE_EDIT);

    roughnessEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        CRect(135, 250, 200, 270), this, ROUGHNESS_EDIT);

    shininessEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        CRect(335, 250, 400, 270), this, SHININESS_EDIT);

    return 0;
}

void CUniformMaterialDialog::OnPaint()
{
    CPaintDC dc(this);
    dc.SetBkMode(TRANSPARENT);

    CRect diffuseRedRect(10, 10, 200, 30);
    dc.DrawText(CString("Diffuse Red:"), -1, &diffuseRedRect, DT_SINGLELINE);

    CRect diffuseGreenRect(10, 50, 200, 70);
    dc.DrawText(CString("Diffuse Green:"), -1, &diffuseGreenRect, DT_SINGLELINE);

    CRect diffuseBlueRec(10, 90, 200, 110);
    dc.DrawText(CString("Diffuse Blue:"), -1, &diffuseBlueRec, DT_SINGLELINE);

    CRect specularRedRect(220, 10, 400, 30);
    dc.DrawText(CString("Specular Red:"), -1, &specularRedRect, DT_SINGLELINE);

    CRect specularGreenRect(220, 50, 400, 70);
    dc.DrawText(CString("Specular Green:"), -1, &specularGreenRect, DT_SINGLELINE);

    CRect specularBlueRect(220, 90, 400, 110);
    dc.DrawText(CString("Specular Blue:"), -1, &specularBlueRect, DT_SINGLELINE);

    CRect emissiveRedRect(10, 130, 200, 150);
    dc.DrawText(CString("Emissive Red:"), -1, &emissiveRedRect, DT_SINGLELINE);

    CRect emissiveGreenRect(10, 170, 200, 190);
    dc.DrawText(CString("Emissive Green:"), -1, &emissiveGreenRect, DT_SINGLELINE);

    CRect emissiveBlueRec(10, 210, 200, 230);
    dc.DrawText(CString("Emissive Blue:"), -1, &emissiveBlueRec, DT_SINGLELINE);

    CRect ambientRedRect(220, 130, 400, 150);
    dc.DrawText(CString("Ambient Red:"), -1, &ambientRedRect, DT_SINGLELINE);

    CRect ambientGreenRect(220, 170, 400, 190);
    dc.DrawText(CString("Ambient Green:"), -1, &ambientGreenRect, DT_SINGLELINE);

    CRect ambientBlueRect(220, 210, 400, 230);
    dc.DrawText(CString("Ambient Blue:"), -1, &ambientBlueRect, DT_SINGLELINE);

    CRect roughnessRect(10, 250, 200, 270);
    dc.DrawText(CString("Roughness:"), -1, &roughnessRect, DT_SINGLELINE);

    CRect shininessRect(220, 250, 400, 270);
    dc.DrawText(CString("Shininess:"), -1, &shininessRect, DT_SINGLELINE);

    diffuseRedEdit.SetFocus();
}

// -------------------------
//    Class CAmbientLightDialog
// -------------------------

enum {
    RED_EDIT = 210,
    GREEN_EDIT,
    BLUE_EDIT,
    BRIGHTNESS_EDIT,
    LIGHT_X_EDIT,
    LIGHT_Y_EDIT,
    LIGHT_Z_EDIT
};

void CAmbientLightDialog::DoDataExchange(CDataExchange* pDX)
{
    CInputDialog::DoDataExchange(pDX);
    DDX_Text(pDX, RED_EDIT, red);
    DDX_Text(pDX, GREEN_EDIT, green);
    DDX_Text(pDX, BLUE_EDIT, blue);
    DDX_Text(pDX, BRIGHTNESS_EDIT, brightness);
}

// CAmbientLightDialog message handlers
BEGIN_MESSAGE_MAP(CAmbientLightDialog, CInputDialog)
    ON_WM_CREATE()
    ON_WM_PAINT()
END_MESSAGE_MAP()

int CAmbientLightDialog::OnCreate(LPCREATESTRUCT lpcs)
{
    redEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        CRect(65, 10, 200, 30), this, RED_EDIT);

    greenEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        CRect(65, 50, 200, 70), this, GREEN_EDIT);

    blueEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        CRect(65, 90, 200, 110), this, BLUE_EDIT);

    brightnessEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        CRect(300, 50, 400, 70), this, BRIGHTNESS_EDIT);

    return 0;
}

void CAmbientLightDialog::OnPaint()
{
    CPaintDC dc(this);
    dc.SetBkMode(TRANSPARENT);

    CRect redRect(10, 10, 200, 30);
    dc.DrawText(CString("Red:"), -1, &redRect, DT_SINGLELINE);

    CRect greenRect(10, 50, 200, 70);
    dc.DrawText(CString("Green:"), -1, &greenRect, DT_SINGLELINE);

    CRect blueRec(10, 90, 200, 110);
    dc.DrawText(CString("Blue:"), -1, &blueRec, DT_SINGLELINE);

    CRect brightnessRec(220, 50, 400, 70);
    dc.DrawText(CString("Brightness:"), -1, &brightnessRec, DT_SINGLELINE);

    redEdit.SetFocus();
}

// -------------------------
//    Class CFloatsDialog_2x3plus1
// -------------------------

enum {
    F11_EDIT = 210,
    F12_EDIT,
    F13_EDIT,
    F21_EDIT,
    F22_EDIT,
    F23_EDIT,
    F3_EDIT
};

void CFloatsDialog_2x3plus1::DoDataExchange(CDataExchange* pDX)
{
    CInputDialog::DoDataExchange(pDX);
    DDX_Text(pDX, F11_EDIT, f11);
    DDX_Text(pDX, F12_EDIT, f12);
    DDX_Text(pDX, F13_EDIT, f13);
    DDX_Text(pDX, F21_EDIT, f21);
    DDX_Text(pDX, F22_EDIT, f22);
    DDX_Text(pDX, F23_EDIT, f23);
    DDX_Text(pDX, F3_EDIT, f3);
}

// CFloatsDialog_2x3plus1 message handlers
BEGIN_MESSAGE_MAP(CFloatsDialog_2x3plus1, CInputDialog)
    ON_WM_CREATE()
    ON_WM_PAINT()
END_MESSAGE_MAP()

int CFloatsDialog_2x3plus1::OnCreate(LPCREATESTRUCT lpcs)
{
    f11Edit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        CRect(100, 10, 200, 30), this, F11_EDIT);

    f12Edit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        CRect(100, 50, 200, 70), this, F12_EDIT);

    f13Edit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        CRect(100, 90, 200, 110), this, F13_EDIT);

    f21Edit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        CRect(335, 10, 400, 30), this, F21_EDIT);

    f22Edit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        CRect(335, 50, 400, 70), this, F22_EDIT);

    f23Edit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        CRect(335, 90, 400, 110), this, F23_EDIT);

    f3Edit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        CRect(230, 130, 300, 150), this, F3_EDIT);

    return 0;
}

void CFloatsDialog_2x3plus1::OnPaint()
{
    CPaintDC dc(this);
    dc.SetBkMode(TRANSPARENT);

    CRect f11Rect(10, 10, 200, 30);
    dc.DrawText(CString(f11Name.c_str()), -1, &f11Rect, DT_SINGLELINE);

    CRect f12Rect(10, 50, 200, 70);
    dc.DrawText(CString(f12Name.c_str()), -1, &f12Rect, DT_SINGLELINE);

    CRect f13Rec(10, 90, 200, 110);
    dc.DrawText(CString(f13Name.c_str()), -1, &f13Rec, DT_SINGLELINE);

    CRect f21Rect(220, 10, 400, 30);
    dc.DrawText(CString(f21Name.c_str()), -1, &f21Rect, DT_SINGLELINE);

    CRect f22Rect(220, 50, 400, 70);
    dc.DrawText(CString(f22Name.c_str()), -1, &f22Rect, DT_SINGLELINE);

    CRect f23Rect(220, 90, 400, 110);
    dc.DrawText(CString(f23Name.c_str()), -1, &f23Rect, DT_SINGLELINE);

    CRect f3Rect(120, 130, 300, 150);
    dc.DrawText(CString(f3Name.c_str()), -1, &f3Rect, DT_SINGLELINE);

    f11Edit.SetFocus();
}

// -------------------------
//    Class CBloomDialog
// -------------------------

enum {
    THRESH_EDIT = 210,
    SPREAD_EDIT,
};

void CBloomDialog::DoDataExchange(CDataExchange* pDX)
{
    CInputDialog::DoDataExchange(pDX);
    DDX_Text(pDX, THRESH_EDIT, thresh);
    DDX_Text(pDX, SPREAD_EDIT, spread);
}

// CBloomDialog message handlers
BEGIN_MESSAGE_MAP(CBloomDialog, CInputDialog)
    ON_WM_CREATE()
    ON_WM_PAINT()
END_MESSAGE_MAP()

int CBloomDialog::OnCreate(LPCREATESTRUCT lpcs)
{
    threshEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        CRect(65, 10, 200, 30), this, THRESH_EDIT);

    spreadEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        CRect(265, 10, 400, 30), this, SPREAD_EDIT);

    return 0;
}

void CBloomDialog::OnPaint()
{
    CPaintDC dc(this);
    dc.SetBkMode(TRANSPARENT);

    CRect threshRect(10, 10, 200, 30);
    dc.DrawText(CString("Thresh:"), -1, &threshRect, DT_SINGLELINE);

    CRect spreadRect(220, 10, 400, 30);
    dc.DrawText(CString("Spread:"), -1, &spreadRect, DT_SINGLELINE);

    threshEdit.SetFocus();
}

// -------------------------
//    Class CFogDialog
// -------------------------

enum {
    FOG_RED_EDIT = 210,
    FOG_GREEN_EDIT,
    FOG_BLUE_EDIT,
    FOG_MIN_EDIT,
    FOG_MAX_EDIT
};

void CFogDialog::DoDataExchange(CDataExchange* pDX)
{
    CInputDialog::DoDataExchange(pDX);
    DDX_Text(pDX, FOG_RED_EDIT, red);
    DDX_Text(pDX, FOG_GREEN_EDIT, green);
    DDX_Text(pDX, FOG_BLUE_EDIT, blue);
    DDX_Text(pDX, FOG_MIN_EDIT, minDist);
    DDX_Text(pDX, FOG_MAX_EDIT, maxDist);
}

// CFogDialog message handlers
BEGIN_MESSAGE_MAP(CFogDialog, CInputDialog)
    ON_WM_CREATE()
    ON_WM_PAINT()
END_MESSAGE_MAP()

int CFogDialog::OnCreate(LPCREATESTRUCT lpcs)
{
    redEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        CRect(65, 10, 200, 30), this, FOG_RED_EDIT);

    greenEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        CRect(65, 50, 200, 70), this, FOG_GREEN_EDIT);

    blueEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        CRect(65, 90, 200, 110), this, FOG_BLUE_EDIT);

    minDistEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        CRect(350, 30, 425, 50), this, FOG_MIN_EDIT);

    maxDistEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        CRect(350, 70, 425, 90), this, FOG_MAX_EDIT);

    return 0;
}

void CFogDialog::OnPaint()
{
    CPaintDC dc(this);
    dc.SetBkMode(TRANSPARENT);

    CRect redRect(10, 10, 200, 30);
    dc.DrawText(CString("Red:"), -1, &redRect, DT_SINGLELINE);

    CRect greenRect(10, 50, 200, 70);
    dc.DrawText(CString("Green:"), -1, &greenRect, DT_SINGLELINE);

    CRect blueRec(10, 90, 200, 110);
    dc.DrawText(CString("Blue:"), -1, &blueRec, DT_SINGLELINE);

    CRect minRect(220, 30, 400, 50);
    dc.DrawText(CString("Minimum Distance:"), -1, &minRect, DT_SINGLELINE);

    CRect maxRect(220, 70, 400, 90);
    dc.DrawText(CString("Maximum Distance:"), -1, &maxRect, DT_SINGLELINE);

    redEdit.SetFocus();
}

// -------------------------
//    Class CToonDialog
// -------------------------

enum {
    TOON_RED_EDIT = 210,
    TOON_GREEN_EDIT,
    TOON_BLUE_EDIT,
    TOON_SHADES_EDIT,
    TOON_THICKNESS_EDIT
};

void CToonDialog::DoDataExchange(CDataExchange* pDX)
{
    CInputDialog::DoDataExchange(pDX);
    DDX_Text(pDX, TOON_RED_EDIT, red);
    DDX_Text(pDX, TOON_GREEN_EDIT, green);
    DDX_Text(pDX, TOON_BLUE_EDIT, blue);
    DDX_Text(pDX, TOON_SHADES_EDIT, shades);
    DDX_Text(pDX, TOON_THICKNESS_EDIT, thickness);
}

// CToonDialog message handlers
BEGIN_MESSAGE_MAP(CToonDialog, CInputDialog)
    ON_WM_CREATE()
    ON_WM_PAINT()
END_MESSAGE_MAP()

int CToonDialog::OnCreate(LPCREATESTRUCT lpcs)
{
    redEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        CRect(100, 10, 200, 30), this, TOON_RED_EDIT);

    greenEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        CRect(100, 50, 200, 70), this, TOON_GREEN_EDIT);

    blueEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        CRect(100, 90, 200, 110), this, TOON_BLUE_EDIT);

    shadesEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        CRect(350, 30, 425, 50), this, TOON_SHADES_EDIT);

    thicknessEdit.Create(ES_MULTILINE | WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,
        CRect(350, 70, 425, 90), this, TOON_THICKNESS_EDIT);

    return 0;
}

void CToonDialog::OnPaint()
{
    CPaintDC dc(this);
    dc.SetBkMode(TRANSPARENT);

    CRect redRect(10, 10, 200, 30);
    dc.DrawText(CString("Border Red:"), -1, &redRect, DT_SINGLELINE);

    CRect greenRect(10, 50, 200, 70);
    dc.DrawText(CString("Border Green:"), -1, &greenRect, DT_SINGLELINE);

    CRect blueRec(10, 90, 200, 110);
    dc.DrawText(CString("Border Blue:"), -1, &blueRec, DT_SINGLELINE);

    CRect shadesRect(220, 30, 400, 50);
    dc.DrawText(CString("Shades:"), -1, &shadesRect, DT_SINGLELINE);

    CRect thicknessRect(220, 70, 400, 90);
    dc.DrawText(CString("Border Thickness:"), -1, &thicknessRect, DT_SINGLELINE);

    redEdit.SetFocus();
}

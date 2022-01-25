
#pragma once

#include <string>
#include "vec.h"
using std::string;

// ------------------------
//    Class CInputDialog
// ------------------------

class CInputDialog : public CDialog
{
    DECLARE_DYNAMIC(CInputDialog)

public:
    CInputDialog(CString title = _T("Input Dialog"));
    virtual ~CInputDialog();

    virtual BOOL OnInitDialog();

    enum { IDD = IDD_INPUTDIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);

private:
    CString mTitle;
};

// ----------------------
//    Class CCmdDialog
// ----------------------

class CCmdDialog : public CInputDialog
{
public:
    CCmdDialog(CString title = _T("Input Dialog"));
    virtual ~CCmdDialog();

    string GetCmd();

protected:
    CString mCmd;
    CEdit mCmdEdit;

    virtual void DoDataExchange(CDataExchange* pDX);

    afx_msg int OnCreate (LPCREATESTRUCT lpcs);
    afx_msg void OnPaint();
    DECLARE_MESSAGE_MAP()
};

// ----------------------
//    Class CXyzDialog
// ----------------------

class CXyzDialog : public CInputDialog
{
public:
    CXyzDialog(CString title = _T("Input Dialog"), float _mX = 0, float _mY = 0, float _mZ = 0);
    void setText(std::string _textX, std::string _textY, std::string _textZ);
    virtual ~CXyzDialog();

    vec3 GetXYZ();

protected:
    float mX;
    float mY;
    float mZ;
    CEdit mXEdit;
    CEdit mYEdit;
    CEdit mZEdit;

    std::string textX, textY, textZ;

    virtual void DoDataExchange(CDataExchange* pDX);

    afx_msg int OnCreate (LPCREATESTRUCT lpcs);
    afx_msg void OnPaint();
    DECLARE_MESSAGE_MAP()
};

// -------------------------
//    Class CCmdXyzDialog
// -------------------------

class CCmdXyzDialog : public CInputDialog
{
public:
    CCmdXyzDialog(CString title = _T("Input Dialog"));
    virtual ~CCmdXyzDialog();

    string GetCmd();
    vec3 GetXYZ();

protected:
    CString mCmd;
    float mX;
    float mY;
    float mZ;
    CEdit mCmdEdit;
    CEdit mXEdit;
    CEdit mYEdit;
    CEdit mZEdit;

    virtual void DoDataExchange(CDataExchange* pDX);

    afx_msg int OnCreate (LPCREATESTRUCT lpcs);
    afx_msg void OnPaint();
    DECLARE_MESSAGE_MAP()
};

// well theoretically we could make do with these dialogs, but meh... that ain't enough for me

class CProjectionDialog : public CInputDialog
{
public:
    CProjectionDialog(CString title) : CInputDialog(title) { }
    CProjectionDialog(CString title, float _bottom, float _top, float _left, float _right, float _zNear, float _zFar)
        : CInputDialog(title), bottom(_bottom), top(_top), left(_left), right(_right), zNear(_zNear), zFar(_zFar) { }
    virtual ~CProjectionDialog() { }

    float getBottom() noexcept { return bottom; }
    float getTop() noexcept { return top; }
    float getLeft() noexcept { return left; }
    float getRight() noexcept { return right; }
    float getNear() noexcept { return zNear; }
    float getFar() noexcept { return zFar; }

protected:
    float bottom = 0, top = 0, left = 0, right = 0, zNear = 0, zFar = 0;
    CEdit bottomEdit, topEdit, leftEdit, rightEdit, zNearEdit, zFarEdit;

    virtual void DoDataExchange(CDataExchange* pDX);

    afx_msg int OnCreate(LPCREATESTRUCT lpcs);
    afx_msg void OnPaint();
    DECLARE_MESSAGE_MAP()
};

class CPrespectiveProjectionDialog : public CInputDialog
{
public:
    CPrespectiveProjectionDialog(CString title) : CInputDialog(title) { }
    CPrespectiveProjectionDialog(CString title, float _fovy, float _aspect, float _zNear, float _zFar)
        : CInputDialog(title), fovy(_fovy), aspect(_aspect), zNear(_zNear), zFar(_zFar) { }
    virtual ~CPrespectiveProjectionDialog() { }

    float getFovY() noexcept { return fovy; }
    float getAspect() noexcept { return aspect; }
    float getNear() noexcept { return zNear; }
    float getFar() noexcept { return zFar; }

protected:
    float fovy = 0, aspect = 0, zNear = 0, zFar = 0;
    CEdit fovyEdit, aspectEdit, zNearEdit, zFarEdit;

    virtual void DoDataExchange(CDataExchange* pDX);

    afx_msg int OnCreate(LPCREATESTRUCT lpcs);
    afx_msg void OnPaint();
    DECLARE_MESSAGE_MAP()
};

class CSingleFloatDialog : public CInputDialog
{
public:
    CSingleFloatDialog(CString title) : CInputDialog(title) { }
    CSingleFloatDialog(CString title, float value, bool isInt = false) : CInputDialog(title), isInt(isInt) {
        if (isInt) iValue = value;
        else fValue = value;
    }
    virtual ~CSingleFloatDialog() { }

    float getValue() noexcept { return isInt ? iValue : fValue; }

protected:
    bool isInt;
    float fValue = 0;
    int iValue = 0;
    CEdit valueEdit;

    virtual void DoDataExchange(CDataExchange* pDX);

    afx_msg int OnCreate(LPCREATESTRUCT lpcs);
    afx_msg void OnPaint();
    DECLARE_MESSAGE_MAP()
};

class CUniformMaterialDialog : public CInputDialog
{
public:
    CUniformMaterialDialog(CString title) : CInputDialog(title) { }
    CUniformMaterialDialog(CString title, float ambientRed, float ambientGreen, float ambientBlue,
        float diffuseRed, float diffuseGreen, float diffuseBlue,
        float specularRed, float specularGreen, float specularBlue,
        float emissiveRed, float emissiveGreen, float emissiveBlue,
        float roughness, float shininess)
        : CInputDialog(title), diffuseRed(diffuseRed), diffuseGreen(diffuseGreen), diffuseBlue(diffuseBlue),
        specularRed(specularRed), specularGreen(specularGreen), specularBlue(specularBlue),
        emissiveRed(emissiveRed), emissiveGreen(emissiveGreen), emissiveBlue(emissiveBlue),
        ambientRed(ambientRed), ambientGreen(ambientGreen), ambientBlue(ambientBlue),
        roughness(roughness), shininess(shininess) { }
    virtual ~CUniformMaterialDialog() { }

    float getAmbientRed() noexcept { return ambientRed; }
    float getAmbientGreen() noexcept { return ambientGreen; }
    float getAmbientBlue() noexcept { return ambientBlue; }
    float getDiffuseRed() noexcept { return diffuseRed; }
    float getDiffuseGreen() noexcept { return diffuseGreen; }
    float getDiffuseBlue() noexcept { return diffuseBlue; }
    float getSpecularRed() noexcept { return specularRed; }
    float getSpecularGreen() noexcept { return specularGreen; }
    float getSpecularBlue() noexcept { return specularBlue; }
    float getEmissiveRed() noexcept { return emissiveRed; }
    float getEmissiveGreen() noexcept { return emissiveGreen; }
    float getEmissiveBlue() noexcept { return emissiveBlue; }
    float getRoughness() noexcept { return roughness; }
    float getShininess() noexcept { return shininess; }

protected:
    float ambientRed = 0, ambientGreen = 0, ambientBlue = 0,
          diffuseRed = 0, diffuseGreen = 0, diffuseBlue = 0,
          specularRed = 0, specularGreen = 0, specularBlue = 0,
          emissiveRed = 0, emissiveGreen = 0, emissiveBlue = 0,
          roughness = 0, shininess = 0;
    CEdit ambientRedEdit, ambientGreenEdit, ambientBlueEdit,
          diffuseRedEdit, diffuseGreenEdit, diffuseBlueEdit,
          specularRedEdit, specularGreenEdit, specularBlueEdit,
          emissiveRedEdit, emissiveGreenEdit, emissiveBlueEdit,
          roughnessEdit, shininessEdit;

    virtual void DoDataExchange(CDataExchange* pDX);

    afx_msg int OnCreate(LPCREATESTRUCT lpcs);
    afx_msg void OnPaint();
    DECLARE_MESSAGE_MAP()
};

class CAmbientLightDialog : public CInputDialog
{
public:
    CAmbientLightDialog(CString title) : CInputDialog(title) { }
    CAmbientLightDialog(CString title, float red, float green, float blue, float brightness)
        : CInputDialog(title), red(red), green(green), blue(blue), brightness(brightness) { }
    virtual ~CAmbientLightDialog() { }

    float getRed() noexcept { return red; }
    float getGreen() noexcept { return green; }
    float getBlue() noexcept { return blue; }
    float getBrightness() noexcept { return brightness; }

protected:
    float red = 0, green = 0, blue = 0, brightness = 0;
    CEdit redEdit, greenEdit, blueEdit, brightnessEdit;

    virtual void DoDataExchange(CDataExchange* pDX);

    afx_msg int OnCreate(LPCREATESTRUCT lpcs);
    afx_msg void OnPaint();
    DECLARE_MESSAGE_MAP()
};

class CFloatsDialog_2x3plus1 : public CInputDialog
{
public:
    CFloatsDialog_2x3plus1(CString title) : CInputDialog(title) { }
    CFloatsDialog_2x3plus1(CString title, std::string f11Name, float f11, std::string f12Name, float f12, std::string f13Name, float f13,
        std::string f21Name, float f21, std::string f22Name, float f22, std::string f23Name, float f23,
        std::string f3Name, float f3)
        : CInputDialog(title), f11Name(f11Name), f11(f11), f12Name(f12Name), f12(f12), f13Name(f13Name), f13(f13),
        f21Name(f21Name), f21(f21), f22Name(f22Name), f22(f22), f23Name(f23Name), f23(f23),
        f3Name(f3Name), f3(f3) { }
    virtual ~CFloatsDialog_2x3plus1() { }

    float getF11() noexcept { return f11; }
    float getF12() noexcept { return f12; }
    float getF13() noexcept { return f13; }
    float getF21() noexcept { return f21; }
    float getF22() noexcept { return f22; }
    float getF23() noexcept { return f23; }
    float getF3() noexcept { return f3; }

protected:
    float f11 = 0, f12 = 0, f13 = 0, f21 = 0, f22 = 0, f23 = 0, f3 = 0;
    std::string f11Name, f12Name, f13Name, f21Name, f22Name, f23Name, f3Name;
    CEdit f11Edit, f12Edit, f13Edit, f21Edit, f22Edit, f23Edit, f3Edit;

    virtual void DoDataExchange(CDataExchange* pDX);

    afx_msg int OnCreate(LPCREATESTRUCT lpcs);
    afx_msg void OnPaint();
    DECLARE_MESSAGE_MAP()
};

class CBloomDialog : public CInputDialog
{
public:
    CBloomDialog(CString title) : CInputDialog(title) { }
    CBloomDialog(CString title, float thresh, int spread) : CInputDialog(title), thresh(thresh), spread(spread) { }
    virtual ~CBloomDialog() { }

    float getThresh() noexcept { return thresh; }
    int getSpread() noexcept { return spread; }

protected:
    float thresh = 0;
    int spread = 0;
    CEdit threshEdit, spreadEdit;

    virtual void DoDataExchange(CDataExchange* pDX);

    afx_msg int OnCreate(LPCREATESTRUCT lpcs);
    afx_msg void OnPaint();
    DECLARE_MESSAGE_MAP()
};

class CFogDialog : public CInputDialog
{
public:
    CFogDialog(CString title) : CInputDialog(title) { }
    CFogDialog(CString title, float red, float green, float blue, float minDist, float maxDist)
        : CInputDialog(title), red(red), green(green), blue(blue), minDist(minDist), maxDist(maxDist) { }
    virtual ~CFogDialog() { }

    float getRed() noexcept { return red; }
    float getGreen() noexcept { return green; }
    float getBlue() noexcept { return blue; }
    float getMinDistance() noexcept { return minDist; }
    float getMaxDistance() noexcept { return maxDist; }

protected:
    float red = 0, green = 0, blue = 0, minDist = 0, maxDist = 0;
    CEdit redEdit, greenEdit, blueEdit, minDistEdit, maxDistEdit;

    virtual void DoDataExchange(CDataExchange* pDX);

    afx_msg int OnCreate(LPCREATESTRUCT lpcs);
    afx_msg void OnPaint();
    DECLARE_MESSAGE_MAP()
};

class CToonDialog : public CInputDialog
{
public:
    CToonDialog(CString title) : CInputDialog(title) { }
    CToonDialog(CString title, float red, float green, float blue, int shades, float thickness)
        : CInputDialog(title), red(red), green(green), blue(blue), shades(shades), thickness(thickness) { }
    virtual ~CToonDialog() { }

    float getRed() noexcept { return red; }
    float getGreen() noexcept { return green; }
    float getBlue() noexcept { return blue; }
    float getShades() noexcept { return shades; }
    float getThickness() noexcept { return thickness; }

protected:
    float red = 0, green = 0, blue = 0, thickness = 0;
    int shades = 0;
    CEdit redEdit, greenEdit, blueEdit, shadesEdit, thicknessEdit;

    virtual void DoDataExchange(CDataExchange* pDX);

    afx_msg int OnCreate(LPCREATESTRUCT lpcs);
    afx_msg void OnPaint();
    DECLARE_MESSAGE_MAP()
};

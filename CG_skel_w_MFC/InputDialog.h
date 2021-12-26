
#pragma once

#include <string>
using std::string;

#include "vec.h"

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
    CSingleFloatDialog(CString title, float _value) : CInputDialog(title), value(_value) { }
    virtual ~CSingleFloatDialog() { }

    float getValue() noexcept { return value; }

protected:
    float value = 0;
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
    CUniformMaterialDialog(CString title, float baseRed, float baseGreen, float baseBlue,
        float emissiveRed, float emissiveGreen, float emissiveBlue,
        float ambientReflect, float roughness, float shininess)
        : CInputDialog(title), baseRed(baseRed), baseGreen(baseGreen), baseBlue(baseBlue),
        emissiveRed(emissiveRed), emissiveGreen(emissiveGreen), emissiveBlue(emissiveBlue),
        ambientReflect(ambientReflect), roughness(roughness), shininess(shininess) { }
    virtual ~CUniformMaterialDialog() { }

    float getBaseRed() noexcept { return baseRed; }
    float getBaseGreen() noexcept { return baseGreen; }
    float getBaseBlue() noexcept { return baseBlue; }
    float getEmissiveRed() noexcept { return emissiveRed; }
    float getEmissiveGreen() noexcept { return emissiveGreen; }
    float getEmissiveBlue() noexcept { return emissiveBlue; }
    float getAmbientReflect() noexcept { return ambientReflect; }
    float getRoughness() noexcept { return roughness; }
    float getShininess() noexcept { return shininess; }

protected:
    float baseRed = 0, baseGreen = 0, baseBlue = 0,
        emissiveRed = 0, emissiveGreen = 0, emissiveBlue = 0,
        ambientReflect = 0, roughness = 0, shininess = 0;
    CEdit baseRedEdit, baseGreenEdit, baseBlueEdit,
        emissiveRedEdit, emissiveGreenEdit, emissiveBlueEdit,
        ambientReflectEdit, roughnessEdit, shininessEdit;

    virtual void DoDataExchange(CDataExchange* pDX);

    afx_msg int OnCreate(LPCREATESTRUCT lpcs);
    afx_msg void OnPaint();
    DECLARE_MESSAGE_MAP()
};

class CRainbowMaterialDialog : public CInputDialog
{
public:
    CRainbowMaterialDialog(CString title) : CInputDialog(title) { }
    CRainbowMaterialDialog(CString title, float ambientReflect, float roughness, float shininess)
        : CInputDialog(title), ambientReflect(ambientReflect), roughness(roughness), shininess(shininess) { }
    virtual ~CRainbowMaterialDialog() { }

    float getAmbientReflect() noexcept { return ambientReflect; }
    float getRoughness() noexcept { return roughness; }
    float getShininess() noexcept { return shininess; }

protected:
    float ambientReflect = 0, roughness = 0, shininess = 0;
    CEdit ambientReflectEdit, roughnessEdit, shininessEdit;

    virtual void DoDataExchange(CDataExchange* pDX);

    afx_msg int OnCreate(LPCREATESTRUCT lpcs);
    afx_msg void OnPaint();
    DECLARE_MESSAGE_MAP()
};

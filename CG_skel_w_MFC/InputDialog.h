
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
    CInputDialog(CString title = "Input Dialog");
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
    CCmdDialog(CString title = "Input Dialog");
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
    CXyzDialog(CString title = "Input Dialog");
    virtual ~CXyzDialog();

    vec3 GetXYZ();

protected:
    float mX;
    float mY;
    float mZ;
    CEdit mXEdit;
    CEdit mYEdit;
    CEdit mZEdit;

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
    CCmdXyzDialog(CString title = "Input Dialog");
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

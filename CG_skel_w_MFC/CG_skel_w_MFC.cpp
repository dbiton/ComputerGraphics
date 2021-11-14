// CG_skel_w_MFC.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CG_skel_w_MFC.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

#include "GL/glew.h"
#include "GL/freeglut.h"
#include "GL/freeglut_ext.h"
#include "vec.h"
#include "mat.h"
#include "InitShader.h"
#include "Scene.h"
#include "Renderer.h"
#include <string>


#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

enum {
    FILE_OPEN = 1
};

enum {
    MAIN_DEMO = 1,
    MAIN_ABOUT = 2,
    MAIN_HELP = 3
};

enum {
    TOGGLE_FACE_NORMALS = 1,
    TOGGLE_VERTEX_NORMALS = 2,
    TOGGLE_BOUNDING_BOX = 3
};

Scene* scene;
Renderer* renderer;

int last_x, last_y;
bool lb_down, rb_down, mb_down;

//----------------------------------------------------------------------------
// Callbacks

void display(void)
{
    //Call the scene and ask it to draw itself
    renderer->ClearColorBuffer();
    renderer->ClearDepthBuffer();
    scene->draw();
}

void reshape(int width, int height)
{
    //update the renderer's buffers
    if (height < 1) height = 1; // if height is too small, OpenGL freaks out
    renderer->CreateBuffers(width, height);
    // need to rebind OpenGL with the new width+height anyway, so might as well
    // at least it seems like we don't need to re-init the whole OpenGL rendering...
}

inline void message(char* message) { AfxMessageBox(_T(message)); }

void errorNoActiveModel() { message("No active model!"); } // TODO add a message specifying how to make a model the active model

void toggleFaceNormals() {
    if (scene->activeModel == -1) errorNoActiveModel();
    else scene->getActiveModel()->ToggleDrawNormalsPerFace();
}

void toggleVertexNormals() {
    if (scene->activeModel == -1) errorNoActiveModel();
    else scene->getActiveModel()->ToggleDrawNormalsPerVert();
}

void toggleBoundingBox() {
    if (scene->activeModel == -1) errorNoActiveModel();
    else scene->getActiveModel()->ToggleDrawBoundingBox();
}

void keyboard(unsigned char key, int x, int y)
{
    float move_coe = 0.1;

    switch (key) {
    // S
    case 115:
        scene->getActiveCamera()->moveBy(vec3(0, move_coe, 0));
        break;
    // W
    case 119:
        scene->getActiveCamera()->moveBy(vec3(0, -move_coe, 0));
        break;
    // D
    case 100:
        scene->getActiveCamera()->moveBy(vec3(-move_coe, 0, 0));
        break;
    // A
    case 97:
        scene->getActiveCamera()->moveBy(vec3(move_coe, 0, 0));
        break;
    // Q
    case 113:
        scene->getActiveCamera()->moveBy(vec3(0, 0, move_coe));
        break;
    // E
    case 101:
        scene->getActiveCamera()->moveBy(vec3(0, 0, -move_coe));
        break;
    case 'f': toggleFaceNormals(); break;
    case 'v': toggleVertexNormals(); break;
    case 'b': toggleBoundingBox(); break;
    case 033:
        exit(EXIT_SUCCESS);
        break;
    }
    display();
}

void mouse(int button, int state, int x, int y)
{
    //button = {GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, GLUT_RIGHT_BUTTON}
    //state = {GLUT_DOWN,GLUT_UP}

    //set down flags
    switch (button) {
    case GLUT_LEFT_BUTTON:
        lb_down = (state == GLUT_UP) ? 0 : 1;
        break;
    case GLUT_RIGHT_BUTTON:
        rb_down = (state == GLUT_UP) ? 0 : 1;
        break;
    case GLUT_MIDDLE_BUTTON:
        mb_down = (state == GLUT_UP) ? 0 : 1;
        break;
    }
}

void motion(int x, int y)
{
    // calc difference in mouse movement
    int dx = x - last_x;
    int dy = y - last_y;
    // update last x,y
    last_x = x;
    last_y = y;

    float rotation_coe = .5f;

    if (rb_down) {
    }
    else if (rb_down) {
    }
    else if (mb_down) {
        scene->getActiveCamera()->rotateBy(rotation_coe * vec3(dy, -dx, 0));
    }
    display();
}

void fileMenu(int id)
{
    switch (id)
    {
    case FILE_OPEN:
        CFileDialog dlg(TRUE, _T(".obj"), NULL, NULL, _T("*.obj|*.*"));
        if (dlg.DoModal() == IDOK)
        {
            std::string s((LPCTSTR)dlg.GetPathName());
            scene->loadOBJModel((LPCTSTR)dlg.GetPathName());
        }
        break;
    }
}

void togglesMenu(int id) {
    switch (id) {
    case TOGGLE_FACE_NORMALS: toggleFaceNormals(); break;
    case TOGGLE_VERTEX_NORMALS: toggleVertexNormals(); break;
    case TOGGLE_BOUNDING_BOX: toggleBoundingBox(); break;
    default: message("NOT IMPLEMENTED GTFO"); break;
    }
    display();
}

void mainMenu(int id)
{
    switch (id)
    {
    case MAIN_DEMO:
        scene->drawDemo(); // oh we're drawing the demo here, didn't notice
        break;
    case MAIN_ABOUT:
        message("Computer Graphics Part 1 - Wireframes\nby Itay Beit Halachmi and Dvir David Biton");
        break;
    case MAIN_HELP:
        message(""); // TODO: Self-documentation!
        break;
    }
}

void initMenu()
{

    int menuFile = glutCreateMenu(fileMenu);
    glutAddMenuEntry("Open..", FILE_OPEN);
    int menuToggles = glutCreateMenu(togglesMenu);
    glutAddMenuEntry("Face Normals", TOGGLE_FACE_NORMALS);
    glutAddMenuEntry("Vertex Normals", TOGGLE_VERTEX_NORMALS);
    glutAddMenuEntry("Bounding Box", TOGGLE_BOUNDING_BOX);
    glutCreateMenu(mainMenu);
    glutAddSubMenu("File", menuFile);
    glutAddSubMenu("Toggle", menuToggles);
    glutAddMenuEntry("Demo", MAIN_DEMO);
    glutAddMenuEntry("About", MAIN_ABOUT);
    glutAddMenuEntry("Help", MAIN_HELP);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}
//----------------------------------------------------------------------------



int my_main(int argc, char** argv)
{
    //----------------------------------------------------------------------------
    // Initialize window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowSize(512, 512);
    glutInitContextVersion(3, 2);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutCreateWindow("CG");
    glewExperimental = GL_TRUE;
    glewInit();
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        /*		...*/
    }
    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));



    renderer = new Renderer(512, 512);
    scene = new Scene(renderer);

    //----------------------------------------------------------------------------
    // Initialize Callbacks

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutReshapeFunc(reshape);
    initMenu();


    glutMainLoop();
    delete scene;
    delete renderer;
    return 0;
}

CWinApp theApp;

using namespace std;

int main(int argc, char** argv)
{
    int nRetCode = 0;

    // initialize MFC and print and error on failure
    if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
    {
        // TODO: change error code to suit your needs
        _tprintf(_T("Fatal Error: MFC initialization failed\n"));
        nRetCode = 1;
    }
    else
    {
        my_main(argc, argv);
    }

    return nRetCode;
}

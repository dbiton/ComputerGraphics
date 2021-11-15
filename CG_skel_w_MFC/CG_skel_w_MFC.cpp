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
    MAIN_DEMO,
    MAIN_ABOUT,
    MAIN_HELP,
    MAIN_SWAP_CONTROL
};

enum {
    TOGGLE_FACE_NORMALS,
    TOGGLE_VERTEX_NORMALS,
    TOGGLE_BOUNDING_BOX,
    TOGGLE_INACTIVES_DIMMING,
    TOGGLE_AXES
};

enum {
    CONTROLLING_CAMERA,
    CONTROLLING_MODEL
};

enum {
    NEW_OBJ,
    NEW_CUBE
};

Scene* scene;
Renderer* renderer;

int last_x, last_y;
bool lb_down, rb_down, mb_down;
int control_mode;

//----------------------------------------------------------------------------
// Callbacks

void display(void)
{
    //Call the scene and ask it to draw itself
    renderer->ClearColorBuffer();
    renderer->ClearDepthBuffer();
    if (scene->activeCamera != -1) scene->draw();
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

Entity* controlled() { return (control_mode == CONTROLLING_CAMERA) ? (Entity*)scene->getActiveCamera() : (Entity*)scene->getActiveModel(); }

void toggleFaceNormals() {
    if (scene->activeModel != -1) scene->getActiveModel()->ToggleDrawNormalsPerFace();
}

void toggleVertexNormals() {
    if (scene->activeModel != -1) scene->getActiveModel()->ToggleDrawNormalsPerVert();
}

void toggleBoundingBox() {
    if (scene->activeModel != -1) scene->getActiveModel()->ToggleDrawBoundingBox();
}

void toggleInactivesDimming() {
    scene->dimInactiveModels = !scene->dimInactiveModels;
}

int menuModels;

void updateMenu() {
    if (control_mode == CONTROLLING_MODEL) glutChangeToMenuEntry(3, "Control Camera", MAIN_SWAP_CONTROL);
    else glutChangeToMenuEntry(3, "Control Model", MAIN_SWAP_CONTROL);
}

void swapControl() {
    if (control_mode == CONTROLLING_CAMERA) control_mode = CONTROLLING_MODEL;
    else control_mode = CONTROLLING_CAMERA;
    updateMenu();
}

void keyboard(unsigned char key, int x, int y)
{
    float move_coe = 0.1;

    switch (key) {

    case 'w': controlled()->moveBy(vec3(0, -move_coe, 0)); break;
    case 'a': controlled()->moveBy(vec3(move_coe, 0, 0)); break;
    case 's': controlled()->moveBy(vec3(0, move_coe, 0)); break;
    case 'd': controlled()->moveBy(vec3(-move_coe, 0, 0)); break;
    case 'q': controlled()->moveBy(vec3(0, 0, move_coe)); break;
    case 'e': controlled()->moveBy(vec3(0, 0, -move_coe)); break;

    case 'f': toggleFaceNormals(); break;
    case 'v': toggleVertexNormals(); break;
    case 'b': toggleBoundingBox(); break;
    case 'i': toggleInactivesDimming(); break;

    case 'c': swapControl(); break;

    case 033: exit(EXIT_SUCCESS); break; // escape

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
        last_x = x;
        last_y = y;
        break;
    case 3: // scrollwheel up
        controlled()->setScale(controlled()->getScale() * 1.1);
        break;
    case 4: // scrollwheel down
        controlled()->setScale(controlled()->getScale() * 0.9);
        break;
    }
    display();
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
        // TODO this code, when applied to a camera, makes it so the entire scene rotates around 0.
        // intentional? maybe should rotate the entire scene around the camera's "looking at" point instead?
        // or should we rotate the "looking at" point itself?
        const Entity* toRotate = controlled();
        const vec3 pos = toRotate->getPosition();
        controlled()->setPosition(vec3(0, 0, 0));
        controlled()->rotateBy(rotation_coe * vec3(dy, -dx, 0));
        controlled()->setPosition(pos);
    }
    display();
}

void newModelMenu(int id) {
    std::string name;
    switch (id) {
    case NEW_OBJ: {
        CFileDialog dlg(TRUE, _T(".obj"), NULL, NULL, _T("*.obj|*.*"));
        if (dlg.DoModal() == IDOK)
        {
            name = dlg.GetFileName();
            scene->loadOBJModel((LPCTSTR)dlg.GetPathName());
        }
        else return; // gonna add a model to the models menu, unless this failed
    } break;
    case NEW_CUBE: // TODO add a dialog for choosing p and dim?
        scene->AddBox(vec3(0, 0, 0), vec3(1, 1, 1));
        name = "Cube";
        break;
    }
    glutSetMenu(menuModels);
    char newEntry[50];
    sprintf(newEntry, "(%d) %s", scene->activeModel, name.c_str());
    glutAddMenuEntry(newEntry, scene->activeModel);
}

void modelsMenu(int id) {
    scene->activeModel = id;
    display();
}

void togglesMenu(int id) {
    switch (id) {
    case TOGGLE_FACE_NORMALS: toggleFaceNormals(); break;
    case TOGGLE_VERTEX_NORMALS: toggleVertexNormals(); break;
    case TOGGLE_BOUNDING_BOX: toggleBoundingBox(); break;
    case TOGGLE_INACTIVES_DIMMING: toggleInactivesDimming(); break;
    case TOGGLE_AXES: // TODO if we want
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
    case MAIN_HELP: // TODO: Self-documentation! if we want
        // message("");
        break;
    case MAIN_SWAP_CONTROL: swapControl(); break;
    }
}

constexpr bool ALLOW_DEMO = false;

void initMenu()
{
    int menuNewModel = glutCreateMenu(newModelMenu);
    glutAddMenuEntry("From OBJ", NEW_OBJ);
    glutAddMenuEntry("Primitive: Cube", NEW_CUBE);
    menuModels = glutCreateMenu(modelsMenu);
    glutAddSubMenu("New...", menuNewModel);

    int menuToggles = glutCreateMenu(togglesMenu);
    glutAddMenuEntry("Face Normals", TOGGLE_FACE_NORMALS);
    glutAddMenuEntry("Vertex Normals", TOGGLE_VERTEX_NORMALS);
    glutAddMenuEntry("Bounding Box", TOGGLE_BOUNDING_BOX);
    glutAddMenuEntry("Inactives Dimming", TOGGLE_INACTIVES_DIMMING); // this will get its name once updateMenu() happens
    glutAddMenuEntry("Axes", TOGGLE_AXES);
    glutCreateMenu(mainMenu);
    glutAddSubMenu("Models", menuModels);
    glutAddSubMenu("Toggle", menuToggles);
    glutAddMenuEntry("3", MAIN_SWAP_CONTROL); // this will get its name once updateMenu() happens
    if (ALLOW_DEMO) glutAddMenuEntry("Demo", MAIN_DEMO);
    glutAddMenuEntry("About", MAIN_ABOUT);
    glutAddMenuEntry("Help", MAIN_HELP);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    updateMenu();
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

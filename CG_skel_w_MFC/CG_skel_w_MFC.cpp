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
#include "InputDialog.h"
#include <string>

#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

enum {
    MAIN_DEMO,
    MAIN_ABOUT,
    MAIN_HELP,
    MAIN_FOCUS,
};

enum {
    TOGGLE_FACE_NORMALS,
    TOGGLE_VERTEX_NORMALS,
    TOGGLE_BOUNDING_BOX,
    TOGGLE_INACTIVES_DIMMING,
    TOGGLE_AXES
};

enum {
	CONTROL_MODEL_IN_WORLD,
	CONTROL_MODEL_IN_MODEL,
    CONTROL_CAMERA_IN_WORLD,
	CONTROL_CAMERA_IN_VIEW,

    CONTROL_MODEL_INTUITIVE, // intuitive control modes cuz the above modes are dog
    CONTROL_CAMERA_INTUITIVE,

    CONTROL_ALL // for reset all
};

enum {
    PROJECTION_ORTHO,
    PROJECTION_FRUSTUM
};

enum { // used for intuitive mode
    CONTROL_CONTEXT_NONE, // for resetting
    CONTROL_CONTEXT_MOVE,
    CONTROL_CONTEXT_SCALE,
    CONTROL_CONTEXT_ROTATE
};

enum {
    NEW_OBJ,
    NEW_CUBOID,
    NEW_PYRAMID,
    NEW_PRISM,
    NEW_SPHERE
};

constexpr bool ALLOW_DEMO = false;

Scene* scene;
Renderer* renderer;

int last_x, last_y;
bool lb_down, rb_down, mb_down;
int controlMode = CONTROL_CAMERA_IN_WORLD;
int menuModels;
float move_coe = 0.1, rotation_coe = 1, scale_coe = 1;

//----------------------------------------------------------------------------
// Callbacks

void display(void)
{
    renderer->ClearColorBuffer();
    renderer->ClearDepthBuffer();
    if (scene->activeCamera != -1) scene->draw();
}

void reshape(int width, int height)
{
    // TODO set correct aspect ratio so the scene doesn't stretch out
    if (height < 1) height = 1; // if height is too small, OpenGL freaks out
    renderer->CreateBuffers(width, height); // at least it seems like we don't need to re-init the whole OpenGL rendering...
}

inline void message(const char* message) { AfxMessageBox(_T(message)); }

void toggleFaceNormals() {
    if (scene->activeModel != -1) scene->getActiveModel()->ToggleDrawNormalsPerFace();
}

void toggleVertexNormals() {
    if (scene->activeModel != -1) scene->getActiveModel()->ToggleDrawNormalsPerVert();
}

void toggleBoundingBox() {
    if (scene->activeModel != -1) scene->getActiveModel()->ToggleDrawBoundingBox();
}

void toggleInactivesDimming() noexcept {
    scene->dimInactiveModels = !scene->dimInactiveModels;
}

void toggleAxes() noexcept {
    scene->drawAxes = !scene->drawAxes;
}

mat4& controlled(int context, int control_mode) {
    switch (control_mode) {
    case CONTROL_CAMERA_IN_VIEW: return scene->getActiveCamera()->self;
    case CONTROL_CAMERA_IN_WORLD: return scene->getActiveCamera()->world;
    case CONTROL_MODEL_IN_MODEL: return scene->getActiveModel()->self;
    case CONTROL_MODEL_IN_WORLD: return scene->getActiveModel()->world;

    case CONTROL_CAMERA_INTUITIVE:
        if (context == CONTROL_CONTEXT_MOVE) return scene->getActiveCamera()->world;
        else return scene->getActiveCamera()->self;

    case CONTROL_MODEL_INTUITIVE:
        if (context == CONTROL_CONTEXT_MOVE) return scene->getActiveModel()->world;
        else return scene->getActiveModel()->self;
    }
}

mat4& controlled(int context) {
    return controlled(context, controlMode);
}

void focus(){
    if (scene->activeCamera != -1) { // only if there's a camera to begin with...
        scene->focus();
        display();
    }
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key) {

    case 'w': moveBy(controlled(CONTROL_CONTEXT_MOVE), vec3(0, -move_coe, 0)); break;
    case 'a': moveBy(controlled(CONTROL_CONTEXT_MOVE), vec3(move_coe, 0, 0)); break;
    case 's': moveBy(controlled(CONTROL_CONTEXT_MOVE), vec3(0, move_coe, 0)); break;
    case 'd': moveBy(controlled(CONTROL_CONTEXT_MOVE), vec3(-move_coe, 0, 0)); break;
    case 'q': moveBy(controlled(CONTROL_CONTEXT_MOVE), vec3(0, 0, move_coe)); break;
    case 'e': moveBy(controlled(CONTROL_CONTEXT_MOVE), vec3(0, 0, -move_coe)); break;

    case 'f': toggleFaceNormals(); break;
    case 'v': toggleVertexNormals(); break;
    case 'b': toggleBoundingBox(); break;
    case 'i': toggleInactivesDimming(); break;
    case 'x': toggleAxes(); break;

    case 'r': reset(controlled(controlMode)); break;

    case '=': focus(); break;

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
        scaleBy(controlled(CONTROL_CONTEXT_SCALE), 1 + (0.1 * scale_coe));
        break;
    case 4: // scrollwheel down
        scaleBy(controlled(CONTROL_CONTEXT_SCALE), 1 - (0.1 * scale_coe));
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

    if (rb_down) {
    }
    else if (rb_down) {
    }
    else if (mb_down) {
        rotateBy(controlled(CONTROL_CONTEXT_ROTATE), rotation_coe * vec3(dy, -dx, 0));
    }
    display();
}

void controlMenu(int id) {
    controlMode = id;
}

void resetMenu(int id, bool redisplay) {
    if (id == CONTROL_ALL) {
        resetMenu(CONTROL_MODEL_IN_MODEL, false); // no need to redisplay between consecutive resets...
        resetMenu(CONTROL_MODEL_IN_WORLD, false);
        resetMenu(CONTROL_CAMERA_IN_VIEW, false);
        resetMenu(CONTROL_CAMERA_IN_WORLD, false);
    }
    else reset(controlled(CONTROL_CONTEXT_NONE, id));
    if (redisplay) display();
}

void resetMenu(int id) { resetMenu(id, true); }

void projectionMenu(int id) {
    if (scene->activeCamera == -1) return; // nothing to do!
    Camera* camera = scene->getActiveCamera();
    switch (id) {
    case PROJECTION_ORTHO: {
        CProjectionDialog dialog("Orthographic Projection Parameters", camera->lastBottom, camera->lastTop,
            camera->lastLeft, camera->lastRight,
            camera->lastNear, camera->lastFar);
        if (dialog.DoModal() == IDOK) {
            camera->Ortho(dialog.getLeft(), dialog.getRight(),
                dialog.getBottom(), dialog.getTop(),
                dialog.getNear(), dialog.getFar());
            display();
        }
    } break;
    case PROJECTION_FRUSTUM: {
        CProjectionDialog dialog("Frustum Projection Parameters", camera->lastBottom, camera->lastTop,
            camera->lastLeft, camera->lastRight,
            camera->lastNear, camera->lastFar);
        if (dialog.DoModal() == IDOK) {
            camera->Frustum(dialog.getLeft(), dialog.getRight(),
                dialog.getBottom(), dialog.getTop(),
                dialog.getNear(), dialog.getFar());
            display();
        }
    } break;
    }
}

void newModelMenu(int id) {
    std::string name;
    switch (id) { // all new models will be automatically placed at the camera's LookingAt position
    case NEW_OBJ: {
        CFileDialog dlg(TRUE, _T(".obj"), NULL, NULL, _T("*.obj|*.*"));
        if (dlg.DoModal() == IDOK)
        {
            name = dlg.GetFileName();
            scene->loadOBJModel((LPCTSTR)dlg.GetPathName());
        }
        else return; // gonna add a model to the models menu, unless this failed
    } break;
    case NEW_CUBOID: // TODO add dialogs for choosing dimensions and whatnot
        scene->AddCuboid(vec3(0, 0, 0), vec3(1, 1, 1));
        name = "Cuboid";
        break;

    case NEW_PYRAMID:
        scene->AddPyramid(vec3(0, 0, 0), 1, 1, 4);
        name = "Pyramid";
        break;

    case NEW_PRISM:
        scene->AddPrism(vec3(0, 0, 0), 1, 1, 7);
        name = "Prism";
        break;

    case NEW_SPHERE:
        scene->AddSphere(vec3(0, 0, 0), 1, 8);
        name = "Sphere";
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
    case TOGGLE_AXES: toggleAxes(); break;
    }
    display();
}

void mainMenu(int id)
{
    switch (id)
    {
    case MAIN_DEMO:
        scene->drawDemo();
        break;
    case MAIN_FOCUS:
        focus();
        break;
    case MAIN_ABOUT:
        message("Computer Graphics Part 1 - Wireframes\nby Itay Beit Halachmi and Dvir David Biton");
        break;
    case MAIN_HELP: // TODO: Self-documentation! if we want
        // message("");
        break;
    }
}

void initMenu()
{
    const int menuNewModel = glutCreateMenu(newModelMenu);
    glutAddMenuEntry("From OBJ", NEW_OBJ);
    glutAddMenuEntry("Primitive: Cuboid", NEW_CUBOID);
    glutAddMenuEntry("Primitive: Pyramid", NEW_PYRAMID);
    glutAddMenuEntry("Primitive: Prism", NEW_PRISM);
    //glutAddMenuEntry("Primitive: Sphere", NEW_SPHERE); // TODO currently unimplemented, either remove this option or implement spheres
    menuModels = glutCreateMenu(modelsMenu);
    glutAddSubMenu("New...", menuNewModel);

    const int menuToggles = glutCreateMenu(togglesMenu);
    glutAddMenuEntry("Face Normals", TOGGLE_FACE_NORMALS);
    glutAddMenuEntry("Vertex Normals", TOGGLE_VERTEX_NORMALS);
    glutAddMenuEntry("Bounding Box", TOGGLE_BOUNDING_BOX);
    glutAddMenuEntry("Inactives Dimming", TOGGLE_INACTIVES_DIMMING);
    glutAddMenuEntry("Axes", TOGGLE_AXES);

    const int menuControl = glutCreateMenu(controlMenu);
    glutAddMenuEntry("Model (self frame)", CONTROL_MODEL_IN_MODEL);
    glutAddMenuEntry("Model (world frame)", CONTROL_MODEL_IN_WORLD);
    glutAddMenuEntry("Camera (view frame)", CONTROL_CAMERA_IN_VIEW);
    glutAddMenuEntry("Camera (world frame)", CONTROL_CAMERA_IN_WORLD);
    glutAddMenuEntry("Model (intuitive mode)", CONTROL_MODEL_INTUITIVE);
    glutAddMenuEntry("Camera (intuitive mode)", CONTROL_CAMERA_INTUITIVE);

    const int menuReset = glutCreateMenu(resetMenu);
    glutAddMenuEntry("Model (self frame)", CONTROL_MODEL_IN_MODEL);
    glutAddMenuEntry("Model (world frame)", CONTROL_MODEL_IN_WORLD);
    glutAddMenuEntry("Camera (view frame)", CONTROL_CAMERA_IN_VIEW);
    glutAddMenuEntry("Camera (world frame)", CONTROL_CAMERA_IN_WORLD);
    glutAddMenuEntry("All active frames", CONTROL_ALL);

    const int menuProjection = glutCreateMenu(projectionMenu);
    glutAddMenuEntry("Orthographic", PROJECTION_ORTHO);
    glutAddMenuEntry("Frustum", PROJECTION_FRUSTUM);

    glutCreateMenu(mainMenu);
    glutAddSubMenu("Models", menuModels);
    glutAddSubMenu("Toggle", menuToggles);
    glutAddSubMenu("Control Mode", menuControl);
    glutAddSubMenu("Reset Frame", menuReset);
    glutAddSubMenu("Change Projection", menuProjection);
    glutAddMenuEntry("Focus", MAIN_FOCUS);
    if (ALLOW_DEMO) glutAddMenuEntry("Demo", MAIN_DEMO);
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

#include "stdafx.h"

// CG_skel_w_MFC.cpp : Defines the entry point for the console application.
//

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include <string>
#include "CG_skel_w_MFC.h"
#include "GL/glew.h"
#include "GL/freeglut.h"
#include "GL/freeglut_ext.h"
#include "InitShader.h"
#include "InputDialog.h"
#include "mat.h"
#include "Renderer.h"
#include "Scene.h"
#include "vec.h"

#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

enum {
    NEW_OBJ,
    NEW_CUBOID,
    NEW_PYRAMID,
    NEW_PRISM,
    NEW_SPHERE
};

enum {
    MATERIAL_UNIFORM,
    MATERIAL_FULLSATSPECTRUM,
    MATERIAL_PHYSSPECTRUM
};

enum {
    NEW_CAMERA_DUPLICATE,
    NEW_CAMERA_AT_POS
};

enum {
    NEW_LIGHT_AMBIENT,
    NEW_LIGHT_POINT,
    NEW_LIGHT_PARALLEL
};

enum {
    TOGGLE_FACE_NORMALS,
    TOGGLE_VERTEX_NORMALS,
    TOGGLE_BOUNDING_BOX,
    TOGGLE_WIREFRAME,
    TOGGLE_CAMERAS,
    TOGGLE_LIGHTS,
    TOGGLE_INACTIVES_DIMMING,
    TOGGLE_AXES,
    TOGGLE_BACKSHADOW
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
    PROJ_FRUSTUM,
    PROJ_ORTHO,
    PROJ_PERSPECTIVE
};

enum {
    SENSITIVITY_MOVEMENT,
    SENSITIVITY_SCALING,
    SENSITIVITY_ROTATION
};

enum {
    ADVANCED_ENABLE,
    ADVANCED_DISABLE
};

enum {
    MAIN_DEMO,
    MAIN_ABOUT,
    MAIN_HELP,
    MAIN_FOCUS,
};



enum { // used for intuitive mode
    CONTROL_CONTEXT_NONE, // for resetting
    CONTROL_CONTEXT_MOVE,
    CONTROL_CONTEXT_SCALE,
    CONTROL_CONTEXT_ROTATE
};

constexpr bool ALLOW_DEMO = false;

Scene* scene;
Renderer* renderer;

int last_x, last_y;
bool lb_down, rb_down, mb_down;
int controlMode = CONTROL_CAMERA_IN_WORLD;
int menuMain, menuModels = -1, menuCameras = -1, menuLights = -1;
float move_coe = 1, scale_coe = 1, rotation_coe = 1;

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
    if (height < 1) height = 1; // if height is too small, OpenGL freaks out
    renderer->CreateBuffers(width, height); // at least it seems like we don't need to re-init the whole OpenGL rendering...
}

inline void message(CString message) { AfxMessageBox(message); }

void toggleFaceNormals() noexcept {
    if (scene->activeModel != -1) {
        MeshModel* model = scene->getActiveModel();
        model->draw_normals_per_face = !model->draw_normals_per_face;
    }
}

void toggleVertexNormals() noexcept {
    if (scene->activeModel != -1) {
        MeshModel* model = scene->getActiveModel();
        model->draw_normals_per_vert = !model->draw_normals_per_vert;
    }
}

void toggleBoundingBox() noexcept {
    if (scene->activeModel != -1) {
        MeshModel* model = scene->getActiveModel();
        model->draw_bounding_box = !model->draw_bounding_box;
    }
}

void toggleWireframe() noexcept {
    if (scene->activeModel != -1) {
        MeshModel* model = scene->getActiveModel();
        model->draw_wireframe = !model->draw_wireframe;
    }
}

void toggleCameras() noexcept {
    scene->drawCameras = !scene->drawCameras;
}

void toggleLights() noexcept {
    scene->drawLights = !scene->drawLights;
}

void toggleInactivesDimming() noexcept {
    scene->dimInactives = !scene->dimInactives;
}

void toggleAxes() noexcept {
    scene->drawAxes = !scene->drawAxes;
}

void toggleBackshadow() noexcept {
    renderer->drawBackshadow = !renderer->drawBackshadow;
}

mat4& controlled(int context, int control_mode) noexcept {
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

void focus() {
    if (scene->activeCamera != -1) { // only if there's a camera to begin with...
        scene->focus();
        display();
    }
}

void resetFrame(mat4& frame, int control_mode) {
    reset(frame);
    if (control_mode == CONTROL_MODEL_IN_MODEL || control_mode == CONTROL_MODEL_INTUITIVE) scene->getActiveModel()->Recenter();
}

void resetFrame(mat4& frame) {
    resetFrame(frame, controlMode);
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 33) exit(EXIT_SUCCESS); // escape
    if (scene->activeCamera == -1) return;

    switch (key) {

    case 'w': moveBy(controlled(CONTROL_CONTEXT_MOVE), vec3(0, -move_coe * 0.1, 0)); break;
    case 'a': moveBy(controlled(CONTROL_CONTEXT_MOVE), vec3(move_coe * 0.1, 0, 0)); break;
    case 's': moveBy(controlled(CONTROL_CONTEXT_MOVE), vec3(0, move_coe * 0.1, 0)); break;
    case 'd': moveBy(controlled(CONTROL_CONTEXT_MOVE), vec3(-move_coe * 0.1, 0, 0)); break;
    case 'q': moveBy(controlled(CONTROL_CONTEXT_MOVE), vec3(0, 0, move_coe * 0.1)); break;
    case 'e': moveBy(controlled(CONTROL_CONTEXT_MOVE), vec3(0, 0, -move_coe * 0.1)); break;

    case 'f': toggleFaceNormals(); break;
    case 'v': toggleVertexNormals(); break;
    case 'b': toggleBoundingBox(); break;
    case 'o': toggleWireframe(); break;
    case 'm': toggleCameras(); break;
    case 'l': toggleLights(); break;
    case 'i': toggleInactivesDimming(); break;
    case 'x': toggleAxes(); break;
    case 'k': toggleBackshadow(); break;

    case 'r': resetFrame(controlled(CONTROL_CONTEXT_NONE)); break;

    case '=': focus(); break;

    default: break; // don't care about buttons we haven't registered
    }
    display();
}

void mouse(int button, int state, int x, int y) {
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
        if (scene->activeCamera != -1) {
            scaleBy(controlled(CONTROL_CONTEXT_SCALE), 1 + (0.1 * scale_coe));
        }
        break;
    case 4: // scrollwheel down
        if (scene->activeCamera != -1) {
            scaleBy(controlled(CONTROL_CONTEXT_SCALE), 1 - (0.1 * scale_coe));
        }
        break;
    default: break; // don't care about mouse actions we haven't registered
    }
    display();
}

void motion(int x, int y)
{
    // calc difference in mouse movement
    const int dx = x - last_x,
              dy = y - last_y;
    // update last x,y
    last_x = x;
    last_y = y;

    if (rb_down) {
    }
    else if (rb_down) {
    }
    else if (mb_down) {
        if (scene->activeCamera != -1) {
            rotateBy(controlled(CONTROL_CONTEXT_ROTATE), rotation_coe * vec3(dy, -dx, 0));
        }
    }
    display();
}

void resetMenu(int id, bool redisplay) {
    if (id == CONTROL_ALL) {
        resetMenu(CONTROL_MODEL_IN_MODEL, false); // no need to redisplay between consecutive resets...
        resetMenu(CONTROL_MODEL_IN_WORLD, false);
        resetMenu(CONTROL_CAMERA_IN_VIEW, false);
        resetMenu(CONTROL_CAMERA_IN_WORLD, false);
    }
    else resetFrame(controlled(CONTROL_CONTEXT_NONE, id), id);
    if (redisplay) display();
}

void showHelp() {
    CString help = _T("Wireframe Renderer Manual\n");
    message(help
        + _T("=== Models ==\n")
        + _T("Add new models into the scene in the \"New...\" submenu, using either existing .obj files or preprogrammed primitives,\n")
        + _T("or select one of the existing models in the scene to become the active model, which may be controlled and focused on.\n")
        + _T("\n")
        + _T("=== Cameras ===\n")
        + _T("Duplicate the current camera using the \"New...\" option, or select the active camera from one of the existing cameras.\n")
        + _T("The active camera is the camera that may be controlled. (P.S.: You may resize the window to your heart's content)\n")
        + _T("\n")
        + _T("=== Toggle ===\n")
        + _T("* Face Normals (default off, hotkey f): Toggles showing face normals on the active model.\n")
        + _T("* Vertex Normals (default off, hotkey v): Toggles showing vertex normals on the active model.\n")
        + _T("* Bounding Box (default off, hotkey b): Toggles showing the bounding box of the active model.\n")
        + _T("* Camera Indicators (default off, hotkey m): Toggles rendering other cameras in the scene.\n")
        + _T("* Inactives Dimming (default on, hotkey i): Toggles dimming the colors of models that aren't the active model.\n")
        + _T("* Axes (default off, hotkey x): Toggles showing the world's axes.\n")
        + _T("\n")
        + _T("=== Control Mode ===\n")
        + _T("Select which frame of the active model/camera to be controlled, using wasd+qe for movement, scrollwheel for scaling, and holding middle-click for rotations.\n")
        + _T("There is also \"Intuitive Mode\" for each, which relegates movement to the World, and everything else to the \"Self\".\n")
        + _T("Make sure your keyboard language is in english!\n")
        + _T("\n")
        + _T("=== Reset ===\n")
        + _T("Reset the specified frame of the active model/camera to its origin. May produce weird results. Can also reset all active frames.\n")
        + _T("Use the hotkey 'r' to reset the currently-controlled frame. (resetting during Intuitive Mode will reset the Self frame)\n")
        + _T("\n")
        + _T("=== Change Projection ===\n")
        + _T("Set projection parameters after choosing either an Orthographic projection, or a Frustum (\"Realistic\") projection.\n")
        + _T("Keep in mind that clipping has not (yet) been implemented!\n")
        + _T("\n")
        + _T("=== Sensitivity ===\n")
        + _T("Change the sensitivity of control over movement, scaling, or rotation.\n")
        + _T("Inputting 0 will reset to the default, and inputting negative numbers will result in their absolute value being used.\n")
        + _T("\n")
        + _T("=== Other ===\n")
        + _T("* Focus (hotkey =): Forces the active camera to point directly at the active model. May produce weird results if applied more than once.\n")
        + _T("* About: General info and credit.\n")
        + _T("* Help: You're here!"));
}

//----------------------------------------------------------------------------
// Menus

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
        message(_T("Computer Graphics Part 2 - Simple Renderer\nby Itay Beit Halachmi and Dvir David Biton"));
        break;
    case MAIN_HELP:
        showHelp();
        break;
    default: message(_T("Unimplemented mainMenu option!")); // shouldn't happen!
    }
}

void modelsMenu(int id) {
    if (id == -1) {
        if (scene->activeModel == -1) {
            message(_T("No active model selected!"));
            return;
        }
        scene->getModels()->erase(scene->getModels()->begin() + scene->activeModel);
        if (scene->activeModel >= scene->getModels()->size())
            scene->activeModel = scene->getModels()->size() - 1;
        makeModelsSubMenu();
    }
    else scene->activeModel = id;
    display();
}

void newModelMenu(int id) {
    bool firstUpdate = (scene->getCameras()->size() == 0);
    switch (id) { // all new models will be automatically placed at the camera's LookingAt position
    case NEW_OBJ: {
        CFileDialog dlg(TRUE, _T(".obj"), NULL, NULL, _T("*.obj|*.*"));
        if (dlg.DoModal() == IDOK)
            scene->loadOBJModel(CT2CA(dlg.GetPathName()).m_psz, CT2CA(dlg.GetFileName()).m_psz);
        else return; // gonna add a model to the models menu, unless this failed
    } break;
    case NEW_CUBOID: {
        vec3 dim(1, 1, 1);
        CXyzDialog dlg(_T("Cuboid Dimensions Parameters"), 1, 1, 1);
        dlg.setText("Width", "Height", "Length");
        if (dlg.DoModal() != IDOK) return;
        dim = dlg.GetXYZ();
        scene->AddCuboid(vec3(), dim);
    } break;
    case NEW_PYRAMID: {
        vec3 param;
        CXyzDialog dlg(_T("Pyramid Parameters"), 1, 2, 4);
        dlg.setText("Height", "Radius", "Sides");
        if (dlg.DoModal() != IDOK) return;
        param = dlg.GetXYZ();
        scene->AddPyramid(vec3(), param.x, param.y, param.z);
    } break;
    case NEW_PRISM: {
        vec3 param;
        CXyzDialog dlg(_T("Prism Parameters"), 1, 2, 8);
        dlg.setText("Height", "Radius", "Sides");
        if (dlg.DoModal() != IDOK) return;
        param = dlg.GetXYZ();
        scene->AddPrism(vec3(), param.x, param.y, param.z);
    } break;
    case NEW_SPHERE: {
        int subdivisions = 1;
        CSingleFloatDialog dialog(_T("Sphere Resolution"), subdivisions);
        if (dialog.DoModal() != IDOK) return;
        subdivisions = std::floor(dialog.getValue());
        if (subdivisions < 1) subdivisions = 1;
        scene->AddSphere(vec3(), 1, subdivisions);
    } break;
    default: message(_T("Unimplemented newModelMenu option!")); // shouldn't happen!
    }
    if (firstUpdate) { // the active camera and light will be set later once the model's added
        makeCamerasSubMenu();
        makeLightsSubMenu();
    }
    char newEntry[50];
    sprintf(newEntry, "(%d) %s", scene->activeModel, scene->getActiveModel()->getName().c_str());
    glutSetMenu(menuModels);
    glutAddMenuEntry(newEntry, scene->activeModel);
}

void materialMenu(int id) {
    if (scene->activeModel == -1) {
        message(_T("No active model selected!"));
        return;
    }
    const Material* material = scene->getActiveModel()->material;
    switch (id) {
    case MATERIAL_UNIFORM: {
        CUniformMaterialDialog dialog(_T("Uniform Material Parameters"),
            material->base.x, material->base.y, material->base.z,
            material->emissive.x, material->emissive.y, material->emissive.z,
            material->ambient_reflect, material->roughness, material->shininess);
        if (dialog.DoModal() != IDOK) return;
        delete scene->getActiveModel()->material;
        scene->getActiveModel()->material = new Material(Color(dialog.getBaseRed(), dialog.getBaseGreen(), dialog.getBaseBlue()),
            Color(dialog.getEmissiveRed(), dialog.getEmissiveGreen(), dialog.getEmissiveBlue()),
            dialog.getAmbientReflect(), dialog.getRoughness(), dialog.getShininess());
    } break;
    case MATERIAL_FULLSATSPECTRUM: {
        CNonuniformMaterialDialog dialog(_T("Full Saturation Spectrum Material Parameters"),
            "Ambient Reflect:", material->ambient_reflect,
            "Roughness:", material->roughness,
            "Shininess:", material->shininess,
            "You have unlocked RAINBOW MODE!!!");
        if (dialog.DoModal() != IDOK) return;
        delete scene->getActiveModel()->material;
        scene->getActiveModel()->material = new FullSatSpectrumMaterial(dialog.getX(), dialog.getY(), dialog.getZ());
    } break;
    case MATERIAL_PHYSSPECTRUM: {
        CNonuniformMaterialDialog dialog(_T("Physical Spectrum Material Parameters"),
            "Red:", material->ambient_reflect,
            "Green:", material->roughness,
            "Blue:", material->shininess,
            "you unlocked anti-rainbow mode...");
        if (dialog.DoModal() != IDOK) return;
        delete scene->getActiveModel()->material;
        scene->getActiveModel()->material = new PhysSpectrumMaterial(Color(dialog.getX(), dialog.getY(), dialog.getZ()));
    } break;
    default: message(_T("Unimplemented materialMenu option!")); // shouldn't happen!
    }
    display();
}

void camerasMenu(int id) {
    if (id == -1) {
        if (scene->activeCamera == -1) {
            message(_T("No active camera selected!"));
            return;
        }
        if (scene->getCameras()->size() > 1) {
            scene->getCameras()->erase(scene->getCameras()->begin() + scene->activeCamera);
            if (scene->activeCamera >= scene->getCameras()->size())
                scene->activeCamera = scene->getCameras()->size() - 1;
        }
        else {
            message(_T("Can't delete the last camera!"));
            return;
        }
        makeCamerasSubMenu();
    }
    else scene->activeCamera = id;
    display();
}

void newCameraMenu(int id) {
    if (scene->activeModel == -1) {
        message(_T("A model must be present before adding a camera."));
        return;
    }
    switch (id) {
    case NEW_CAMERA_DUPLICATE: {
        scene->AddCamera(*scene->getActiveCamera());
    } break;
    case NEW_CAMERA_AT_POS: {
        vec3 newPos;
        Camera* newCamera;
        if (scene->activeCamera != -1) {
            newCamera = new Camera(*scene->getActiveCamera());
            newPos = getPosition(newCamera->getTransform());
        }
        else
            newCamera = Camera::DefaultCamera(scene->getActiveModel()->getBoundingBoxMin(), scene->getActiveModel()->getBoundingBoxMax());
        
        CXyzDialog dialog(_T("New Camera Position"), newPos.x, newPos.y, newPos.z);
        dialog.setText("X:", "Y:", "Z:");
        if (dialog.DoModal() != IDOK) return;
        setPosition(newCamera->self, dialog.GetXYZ());
        scene->AddCamera(*newCamera);
    } break;
    default: message(_T("Unimplemented newCameraMenu option!")); // shouldn't happen!
    }
    makeCamerasSubMenu();
    display();
}

void shadingMenu(int id) {
    if (scene->activeCamera != -1) {
        scene->getActiveCamera()->shading = id;
        display();
    }
}

Light* getLight(int type, Light* defaults = NULL) {
    float red = 0, green = 0, blue = 0, brightness = 0;
    vec3 vec(1, 0, 0);
    if (defaults != NULL) {
        red = defaults->getColor().x;
        green = defaults->getColor().y;
        blue = defaults->getColor().z;
        brightness = defaults->getBrightness();
        if (type == LIGHT_POINT) vec = ((PointLight*)defaults)->getPosition();
        else if (type == LIGHT_PARALLEL) vec = ((ParallelLight*)defaults)->getDirection();
    }
    switch (type) {
    case NEW_LIGHT_AMBIENT: {
        CAmbientLightDialog dialog(_T("Ambient Light Parameters"), red, green, blue, brightness);
        if (dialog.DoModal() != IDOK) return NULL;
        return new AmbientLight(Color(dialog.getRed(), dialog.getGreen(), dialog.getBlue()), dialog.getBrightness());
    } break;
    case NEW_LIGHT_POINT: {
        CFloatsDialog_2x3plus1 dialog(_T("Point Light Parameters"), "Red:", red, "Green:", green, "Blue:", blue,
            "Origin X:", vec.x, "Origin Y:", vec.y, "Origin Z:", vec.z,
            "Brightness:", brightness);
        if (dialog.DoModal() != IDOK) return NULL;
        return new PointLight(Color(dialog.getF11(), dialog.getF12(), dialog.getF13()), dialog.getF3(),
            vec3(dialog.getF21(), dialog.getF22(), dialog.getF23()));
    } break;
    case NEW_LIGHT_PARALLEL: {
        CFloatsDialog_2x3plus1 dialog(_T("Point Light Parameters"), "Red:", red, "Green:", green, "Blue:", blue,
            "Direction X:", vec.x, "Direction Y:", vec.y, "Direction Z:", vec.z,
            "Brightness:", brightness);
        if (dialog.DoModal() != IDOK) return NULL;
        vec3 direction(dialog.getF21(), dialog.getF22(), dialog.getF23());
        if (length(direction) < FLT_EPSILON) {
            message(_T("Can't use zero vectors as direction!"));
            return NULL;
        }
        return new ParallelLight(Color(dialog.getF11(), dialog.getF12(), dialog.getF13()), dialog.getF3(), normalize(direction));
    } break;
    default: message(_T("Unknown light type!")); return NULL; // shouldn't happen!
    }
}

void lightsMenu(int id) {
    if (id == -1) {
        if (scene->activeLight == -1) {
            message(_T("No active light selected!"));
            return;
        }
        scene->getLights()->erase(scene->getLights()->begin() + scene->activeLight);
        if (scene->activeLight >= scene->getLights()->size())
            scene->activeLight = scene->getLights()->size() - 1;
        makeLightsSubMenu();
    }
    else scene->activeLight = id;
    display();
}

void newLightMenu(int id) {
    Light* newLight = getLight(id);
    if (newLight != NULL) {
        scene->AddLight(newLight);
        scene->activeLight = scene->getLights()->size() - 1;
        makeLightsSubMenu();
        display();
    }
}

void changeLightMenu(int id) {
    if (scene->activeLight == -1) {
        message(_T("No active light selected!"));
        return;
    }
    Light* activeLight = scene->getActiveLight();
    Light* newLight = getLight(id, activeLight);
    if (newLight != NULL) {
        scene->UpdateActiveLight(newLight);
        display();
    }
}

void togglesMenu(int id) {
    switch (id) {
    case TOGGLE_FACE_NORMALS: toggleFaceNormals(); break;
    case TOGGLE_VERTEX_NORMALS: toggleVertexNormals(); break;
    case TOGGLE_BOUNDING_BOX: toggleBoundingBox(); break;
    case TOGGLE_WIREFRAME: toggleWireframe(); break;
    case TOGGLE_CAMERAS: toggleCameras(); break;
    case TOGGLE_LIGHTS: toggleLights(); break;
    case TOGGLE_INACTIVES_DIMMING: toggleInactivesDimming(); break;
    case TOGGLE_AXES: toggleAxes(); break;
    case TOGGLE_BACKSHADOW: toggleBackshadow(); break;
    default: message(_T("Unimplemented togglesMenu option!")); // shouldn't happen!
    }
    display();
}

void controlMenu(int id) {
    controlMode = id;
}

void resetMenu(int id) {
    resetMenu(id, true);
}

void projectionMenu(int id) {
    if (scene->activeCamera == -1) return; // nothing to do!
    Camera* camera = scene->getActiveCamera();
    switch (id) {
    case PROJ_ORTHO: {
        CProjectionDialog dialog(_T("Orthographic Projection Parameters"), camera->lastBottom, camera->lastTop,
            camera->lastLeft, camera->lastRight,
            camera->lastNear, camera->lastFar);
        if (dialog.DoModal() == IDOK) {
            camera->projection = camera->Ortho(dialog.getLeft(), dialog.getRight(),
                dialog.getBottom(), dialog.getTop(),
                dialog.getNear(), dialog.getFar());
            display();
        }
    } break;
    case PROJ_FRUSTUM: {
        CProjectionDialog dialog(_T("Frustum Projection Parameters"), camera->lastBottom, camera->lastTop,
            camera->lastLeft, camera->lastRight,
            camera->lastNear, camera->lastFar);
        if (dialog.DoModal() == IDOK) {
            camera->projection = camera->Frustum(dialog.getLeft(), dialog.getRight(),
                dialog.getBottom(), dialog.getTop(),
                dialog.getNear(), dialog.getFar());
            display();
        }
    } break;
    case PROJ_PERSPECTIVE: {
        float fovy, aspect;
        camera->getPerspectiveParameters(fovy, aspect);
        CPrespectiveProjectionDialog dialog(_T("Prespective Projection Parameters"),
            fovy, aspect, camera->lastNear, camera->lastFar);
        if (dialog.DoModal() == IDOK) {
            camera->projection = camera->Perspective(dialog.getFovY(), dialog.getAspect(),
                dialog.getNear(), dialog.getFar());
            display();
        }
    } break;
    default: message(_T("Unimplemented projectionMenu option!")); // shouldn't happen!
    }
}

void sensitivityMenu(int id) {
    switch (id) {
    case SENSITIVITY_MOVEMENT: {
        CSingleFloatDialog dialog(_T("Movement Sensitivity"), move_coe);
        if (dialog.DoModal() == IDOK) move_coe = std::abs(dialog.getValue());
        if (move_coe == 0) move_coe = 1;
    } break;
    case SENSITIVITY_SCALING: {
        CSingleFloatDialog dialog(_T("Scaling Sensitivity"), scale_coe);
        if (dialog.DoModal() == IDOK) scale_coe = std::abs(dialog.getValue());
        if (scale_coe == 0) scale_coe = 1;
    } break;
    case SENSITIVITY_ROTATION: {
        CSingleFloatDialog dialog(_T("Rotation Sensitivity"), rotation_coe);
        if (dialog.DoModal() == IDOK) rotation_coe = std::abs(dialog.getValue());
        if (rotation_coe == 0) rotation_coe = 1;
    } break;
    default: message(_T("Unimplemented sensitivityMenu option!")); // shouldn't happen!
    }
}

void advancedMenu(int id) {
    // Nothing currently...
}

void supersamplingMenu(int id) {
    switch (id) {
    case ADVANCED_ENABLE: {
        CSingleFloatDialog dialog(_T("Supersampling factor"), renderer->getSupersamplingFactor(), true);
        if (dialog.DoModal() == IDOK) renderer->setSupersampling(true, dialog.getValue());
    } break;
    case ADVANCED_DISABLE: renderer->setSupersampling(false); break;
    default: message(_T("Unimplemented supersamplingMenu option!")); // shouldn't happen!
    }
    display();
}

void fogMenu(int id) {
    switch (id) {
    case ADVANCED_ENABLE: {
        CFogDialog dialog(_T("Bloom Parameters"), renderer->getFogColor().x, renderer->getFogColor().y, renderer->getFogColor().z,
            renderer->getFogMinDistance(), renderer->getFogMaxDistance());
        if (dialog.DoModal() == IDOK)
            renderer->setFog(true, Color(dialog.getRed(), dialog.getGreen(), dialog.getBlue()),
                dialog.getMinDistance(), dialog.getMaxDistance());
    } break;
    case ADVANCED_DISABLE: renderer->setFog(false); break;
    default: message(_T("Unimplemented fogMenu option!")); // shouldn't happen!
    }
    display();
}

void bloomMenu(int id) {
    switch (id) {
    case ADVANCED_ENABLE: {
        CBloomDialog dialog(_T("Bloom Parameters"), renderer->getThreshBloom(), renderer->getSpreadBloom());
        if (dialog.DoModal() == IDOK) renderer->setBloom(true, dialog.getThresh(), dialog.getSpread());
    } break;
    case ADVANCED_DISABLE: renderer->setBloom(false); break;
    default: message(_T("Unimplemented bloomMenu option!")); // shouldn't happen!
    }
    display();
}

void makeModelsSubMenu() {
    // sadly pointwise deleting the models is more of a pain than just completely remaking the menu
    if (menuModels != -1) {
        const int newMenu = glutCreateMenu(modelsMenu);
        glutSetMenu(menuMain);
        glutChangeToSubMenu(1, "Models", newMenu);
        glutDestroyMenu(menuModels);
        menuModels = newMenu;
    }
    else {
        menuModels = glutCreateMenu(modelsMenu);
        glutSetMenu(menuMain);
        glutAddSubMenu("Models", menuModels);
    }
    const int menuNewModel = glutCreateMenu(newModelMenu),
              menuMaterial = glutCreateMenu(materialMenu);

    // intentional indents to visualize the structure of the menu
    glutSetMenu(menuModels);
    /**/glutAddSubMenu("New...", menuNewModel); glutSetMenu(menuNewModel);
    /*    */glutAddMenuEntry("From OBJ", NEW_OBJ);
    /*    */glutAddMenuEntry("Primitive: Cuboid", NEW_CUBOID);
    /*    */glutAddMenuEntry("Primitive: Pyramid", NEW_PYRAMID);
    /*    */glutAddMenuEntry("Primitive: Prism", NEW_PRISM);
    /*    */glutAddMenuEntry("Primitive: Sphere", NEW_SPHERE);
    /**/glutSetMenu(menuModels);
    /**/glutAddSubMenu("Set Material", menuMaterial); glutSetMenu(menuMaterial);
    /*    */glutAddMenuEntry("Uniform...", MATERIAL_UNIFORM);
    /*    */glutAddMenuEntry("Full Saturation Spectrum", MATERIAL_FULLSATSPECTRUM);
    /*    */glutAddMenuEntry("Physical Spectrum", MATERIAL_PHYSSPECTRUM);
    /**/glutSetMenu(menuModels);
    /**/glutAddMenuEntry("Delete active model", -1);
    /**/for (int i = 0; i < scene->getModels()->size(); i++) {
        /**/char newEntry[50];
        /**/sprintf(newEntry, "(%d) %s", i, (*scene->getModels())[i]->getName().c_str());
        /**/glutAddMenuEntry(newEntry, i);
        }
    glutSetMenu(menuMain);
}

void makeCamerasSubMenu() {
    if (menuCameras != -1) {
        const int newMenu = glutCreateMenu(camerasMenu);
        glutSetMenu(menuMain);
        glutChangeToSubMenu(2, "Cameras", newMenu);
        glutDestroyMenu(menuCameras);
        menuCameras = newMenu;
    }
    else {
        menuCameras = glutCreateMenu(camerasMenu);
        glutSetMenu(menuMain);
        glutAddSubMenu("Cameras", menuCameras);
    }
    const int menuNewCamera = glutCreateMenu(newCameraMenu),
              menuShading = glutCreateMenu(shadingMenu);

    // intentional indents to visualize the structure of the menu
    glutSetMenu(menuCameras);
    /**/glutAddSubMenu("New...", menuNewCamera); glutSetMenu(menuNewCamera);
    /*    */glutAddMenuEntry("Duplicate...", NEW_CAMERA_DUPLICATE);
    /*    */glutAddMenuEntry("At position...", NEW_CAMERA_AT_POS);
    /**/glutSetMenu(menuCameras);
    /**/glutAddSubMenu("Set Shading", menuShading); glutSetMenu(menuShading);
    /*    */glutAddMenuEntry("None", SHADE_NONE);
    /*    */glutAddMenuEntry("Flat", SHADE_FLAT);
    /*    */glutAddMenuEntry("Gouraud", SHADE_GOURAUD);
    /*    */glutAddMenuEntry("Phong", SHADE_PHONG);
    /**/glutSetMenu(menuCameras);
    /**/glutAddMenuEntry("Delete active camera", -1);
    /**/for (int i = 0; i < scene->getCameras()->size(); i++) {
        /**/char newEntry[50];
        /**/sprintf(newEntry, "(%d) Camera", i);
        /**/glutAddMenuEntry(newEntry, i);
        }
    glutSetMenu(menuMain);
}

void makeLightsSubMenu() {
    if (menuLights != -1) {
        const int newMenu = glutCreateMenu(lightsMenu);
        glutSetMenu(menuMain);
        glutChangeToSubMenu(3, "Lights", newMenu);
        glutDestroyMenu(menuLights);
        menuLights = newMenu;
    }
    else {
        menuLights = glutCreateMenu(lightsMenu);
        glutSetMenu(menuMain);
        glutAddSubMenu("Lights", menuLights);
    }
    const int menuNewLight = glutCreateMenu(newLightMenu),
              menuChangeLight = glutCreateMenu(changeLightMenu);

    // intentional indents to visualize the structure of the menu
    glutSetMenu(menuLights);
    /**/glutAddSubMenu("New...", menuNewLight); glutSetMenu(menuNewLight);
    /*    */glutAddMenuEntry("Ambient...", NEW_LIGHT_AMBIENT);
    /*    */glutAddMenuEntry("Point...", NEW_LIGHT_POINT);
    /*    */glutAddMenuEntry("Parallel...", NEW_LIGHT_PARALLEL);
    /**/glutSetMenu(menuLights);
    /**/glutAddSubMenu("Change active light", menuChangeLight); glutSetMenu(menuChangeLight);
    /*    */glutAddMenuEntry("Ambient...", NEW_LIGHT_AMBIENT);
    /*    */glutAddMenuEntry("Point...", NEW_LIGHT_POINT);
    /*    */glutAddMenuEntry("Parallel...", NEW_LIGHT_PARALLEL);
    /**/glutSetMenu(menuLights);
    /**/glutAddMenuEntry("Delete active light", -1);
    /**/for (int i = 0; i < scene->getLights()->size(); i++) {
        /**/char newEntry[50];
        /**/sprintf(newEntry, "(%d) %s", i, (*scene->getLights())[i]->getNameOfType().c_str());
        /**/glutAddMenuEntry(newEntry, i);
        }
    glutSetMenu(menuMain);
}

void initMenu()
{
    // create all the submenus first, then construct everything visually.
    // maybe this is more inefficient, but it's easier to work with
    int menuToggles, menuControl, menuReset, menuProjection,
        menuSensitivity, menuAdvanced, menuSupersampling, menuFog, menuBloom;

    // intentional indents to visualize the structure of the menu
    menuMain = glutCreateMenu(mainMenu);
    /**/// models menu
    /**/// cameras menu
    /**/// lights menu
    /**/menuToggles = glutCreateMenu(togglesMenu);
    /**/menuControl = glutCreateMenu(controlMenu);
    /**/menuReset = glutCreateMenu(resetMenu);
    /**/menuProjection = glutCreateMenu(projectionMenu);
    /**/menuSensitivity = glutCreateMenu(sensitivityMenu);
    /**/menuAdvanced = glutCreateMenu(advancedMenu);
    /*    */menuSupersampling = glutCreateMenu(supersamplingMenu);
    /*    */menuFog = glutCreateMenu(fogMenu);
    /*    */menuBloom = glutCreateMenu(bloomMenu);

    glutSetMenu(menuMain);
    makeModelsSubMenu();
    makeCamerasSubMenu();
    makeLightsSubMenu();
    glutAddSubMenu("Toggle", menuToggles); glutSetMenu(menuToggles);
    /**/glutAddMenuEntry("Face Normals", TOGGLE_FACE_NORMALS);
    /**/glutAddMenuEntry("Vertex Normals", TOGGLE_VERTEX_NORMALS);
    /**/glutAddMenuEntry("Bounding Box", TOGGLE_BOUNDING_BOX);
    /**/glutAddMenuEntry("Wireframe", TOGGLE_WIREFRAME);
    /**/glutAddMenuEntry("Camera Indicators", TOGGLE_CAMERAS);
    /**/glutAddMenuEntry("Light Indicators", TOGGLE_LIGHTS);
    /**/glutAddMenuEntry("Inactives Dimming", TOGGLE_INACTIVES_DIMMING);
    /**/glutAddMenuEntry("Axes", TOGGLE_AXES);
    /**/glutAddMenuEntry("Backshadows", TOGGLE_BACKSHADOW);
    glutSetMenu(menuMain);
    glutAddSubMenu("Control Mode", menuControl); glutSetMenu(menuControl);
    /**/glutAddMenuEntry("Model (self frame)", CONTROL_MODEL_IN_MODEL);
    /**/glutAddMenuEntry("Model (world frame)", CONTROL_MODEL_IN_WORLD);
    /**/glutAddMenuEntry("Camera (view frame)", CONTROL_CAMERA_IN_VIEW);
    /**/glutAddMenuEntry("Camera (world frame)", CONTROL_CAMERA_IN_WORLD);
    /**/glutAddMenuEntry("Model (intuitive mode)", CONTROL_MODEL_INTUITIVE);
    /**/glutAddMenuEntry("Camera (intuitive mode)", CONTROL_CAMERA_INTUITIVE);
    glutSetMenu(menuMain);
    glutAddSubMenu("Reset Frame", menuReset); glutSetMenu(menuReset);
    /**/glutAddMenuEntry("Model (self frame)", CONTROL_MODEL_IN_MODEL);
    /**/glutAddMenuEntry("Model (world frame)", CONTROL_MODEL_IN_WORLD);
    /**/glutAddMenuEntry("Camera (view frame)", CONTROL_CAMERA_IN_VIEW);
    /**/glutAddMenuEntry("Camera (world frame)", CONTROL_CAMERA_IN_WORLD);
    /**/glutAddMenuEntry("All active frames", CONTROL_ALL);
    glutSetMenu(menuMain);
    glutAddSubMenu("Change Projection", menuProjection); glutSetMenu(menuProjection);
    /**/glutAddMenuEntry("Orthographic", PROJ_ORTHO);
    /**/glutAddMenuEntry("Frustum", PROJ_FRUSTUM);
    /**/glutAddMenuEntry("Perspective", PROJ_PERSPECTIVE);
    glutSetMenu(menuMain);
    glutAddSubMenu("Sensitivity", menuSensitivity); glutSetMenu(menuSensitivity);
    /**/glutAddMenuEntry("Movement", SENSITIVITY_MOVEMENT);
    /**/glutAddMenuEntry("Scaling", SENSITIVITY_SCALING);
    /**/glutAddMenuEntry("Rotation", SENSITIVITY_ROTATION);
    glutSetMenu(menuMain);
    glutAddSubMenu("Advanced", menuAdvanced); glutSetMenu(menuAdvanced);
    /**/glutAddSubMenu("Supersampling", menuSupersampling); glutSetMenu(menuSupersampling);
    /*    */glutAddMenuEntry("Enable...", ADVANCED_ENABLE);
    /*    */glutAddMenuEntry("Disable", ADVANCED_DISABLE);
    /**/glutSetMenu(menuAdvanced);
    /**/glutAddSubMenu("Fog", menuFog); glutSetMenu(menuFog);
    /*    */glutAddMenuEntry("Enable...", ADVANCED_ENABLE);
    /*    */glutAddMenuEntry("Disable", ADVANCED_DISABLE);
    /**/glutSetMenu(menuAdvanced);
    /**/glutAddSubMenu("Bloom", menuBloom); glutSetMenu(menuBloom);
    /*    */glutAddMenuEntry("Enable...", ADVANCED_ENABLE);
    /*    */glutAddMenuEntry("Disable", ADVANCED_DISABLE);
    /**/glutSetMenu(menuAdvanced);
    glutSetMenu(menuMain);
    glutAddMenuEntry("Focus", MAIN_FOCUS);
    if (ALLOW_DEMO) glutAddMenuEntry("Demo", MAIN_DEMO);
    glutAddMenuEntry("About", MAIN_ABOUT);
    //glutAddMenuEntry("Help", MAIN_HELP); // no way jose, this'd become way too big if i tried updating it
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
        _tprintf(_T("Fatal Error: MFC initialization failed\n"));
        nRetCode = 1;
    }
    else
    {
        my_main(argc, argv);
    }

    return nRetCode;
}

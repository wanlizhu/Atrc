#include <iostream>

#include <Atrc/Atrc.h>
#include <Utils.h>

using namespace AGZ;
using namespace Atrc;
using namespace std;

// See https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
float ACESFilm(float x)
{
    /*constexpr float a = 2.51f;
    constexpr float b = 0.03f;
    constexpr float c = 2.43f;
    constexpr float d = 0.59f;
    constexpr float e = 0.14f;
    return Clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0f, 1.0f);*/
    return x;
}

Texture2D<Color3b> ToSavedImage(const RenderTarget &origin, float gamma)
{
    return origin.Map([=](const Color3f &color)
    {
        return color.Map([=](float x)
        {
            return static_cast<uint8_t>(Clamp(Pow(ACESFilm(x), gamma), 0.0f, 1.0f) * 255);
        });
    });
}

int main()
{
    constexpr uint32_t SCR_W = 640;
    constexpr uint32_t SCR_H = 480;
    constexpr Real SCR_ASPECT_RATIO = static_cast<Real>(SCR_W) / SCR_H;

    //============= Camera =============

    const Vec3 eye = { -5.0, 0.0, 0.0 };
    const Vec3 dir = Vec3(0.0, 0.0, 0.0) - eye;
    PerspectiveCamera camera(eye, dir, { 0.0, 0.0, 1.0 }, Deg(60.0), SCR_ASPECT_RATIO);

    //============= Scene =============

    Sphere sph(Transform::Translate(0.0, 0.0, -201.0), 200.0);
    DiffuseMaterial groundMat(Spectrum(0.4f, 0.8f, 0.4f));
    GeometricEntity ground(&sph, &groundMat);

    Sphere sph2(Transform::Translate(0.0, 0.0, 0.0), 1.0);
    DiffuseMaterial medMat(Spectrum(0.7f));
    GeometricEntity medSph(&sph2, &medMat);

    Sphere sph3(Transform::Translate(0.0, 2.0, 0.0), 1.0);
    SchlickApproximation leftMatFresnel(1.0f, 1.4f);
    FresnelSpecular leftMat(Spectrum(0.8f), &leftMatFresnel);
    GeometricEntity leftSph(&sph3, &leftMat);

    /*Model::WavefrontObj dragonObj;
    Model::WavefrontObjFile::LoadFromObjFile("./Assets/dragon_vrip.obj", &dragonObj);
    auto dragonBVHCore = MakeRC<TriangleBVHCore>(dragonObj.ToGeometryMeshGroup().submeshes["Default"]);
    dragonObj.Clear();
    TriangleBVH dragonBVH(Transform::Translate(-1.5, -2.8, -0.6) * Transform::RotateZ(Deg(-90.0)) * Transform::Scale(2.2 / 40), dragonBVHCore);
    IdealMirror dragonMat(Spectrum(0.6f), MakeRC<FresnelDielectric>(1.0f, 0.01f));
    GeometricEntity dragon(&dragonBVH, &dragonMat);*/

    Texture2D<Spectrum> cubeTex = Texture2D<Spectrum>(
        TextureFile::LoadRGBFromFile("./Assets/CubeTex.png").Map(
        [](const Color3b &c) { return c.Map([](uint8_t b) { return b / 255.0f; }); }));

    Cube cube(Transform::Translate(0.0, -2.0, 0.123) * Transform::Rotate({ 1.0, 1.1, 1.2 }, Deg(47)), 1.4);
    DiffuseMaterial cubeDiffuse(Spectrum(0.2f, 0.4f, 0.8f));
    TextureScaler<Atrc::LinearSampler> cubeMat(&cubeTex, &cubeDiffuse);
    GeometricEntity rightCube(&cube, &cubeMat);

    SkyLight sky(Spectrum(0.4f, 0.7f, 0.9f), Spectrum(1.0f));

    Scene scene;
    scene.camera    = &camera;
    scene.lights_   = { &sky };
    scene.entities_ = { &medSph, &leftSph, &rightCube, &ground, };

    for(auto ent : scene.GetEntities())
    {
        auto L = ent->AsLight();
        if(L)
            scene.lights_.push_back(L);
    }

    sky.PreprocessScene(scene);

    //============= Render Target =============

    RenderTarget renderTarget(SCR_W, SCR_H);

    //============= Renderer & Integrator =============

    JitteredSubareaRenderer subareaRenderer(1000);

    ParallelRenderer renderer(6);
    //SerialRenderer renderer;
    renderer.SetProgressPrinting(true);

    //PureColorIntegrator integrator(SPECTRUM::BLACK, SPECTRUM::WHITE);
    PathTracer integrator(10);

    //============= Rendering =============

    cout << "Start rendering..." << endl;

    Timer timer;
    renderer.Render(subareaRenderer, scene, integrator, renderTarget);
    auto deltaTime = timer.Milliseconds() / 1000.0;

    cout << "Complete rendering...Total time: " << deltaTime << "s." << endl;

    //============= Output =============

    TextureFile::WriteRGBToPNG("./Build/Output.png", ToSavedImage(renderTarget, 1 / 2.2f));
}

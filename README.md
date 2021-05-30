# Unfolding Shapes

This project aims to take in 3d models and unwrap them onto a 2d plane in one piece with an animation. In other words, it is creating a [geometric net](https://www.onlinemathlearning.com/image-files/nets-of-solids.png) of any 3d model. The project also contains a UI that can manipulate the settings and conditions of the pattern formed by the unfolded shape and can control the animation style of the transformation between the shape and the unfold.

## Importing Project

When cloning the repo and opening the visual studios project file, you must follow several steps.

1. Download the [Qt Online Installer](https://www.qt.io/download-qt-installer). 

2. Make an account and click through the prompts until you reach the installation folder.

3. Select "C:\Qt" as the installation folder and check the custom installation box. Click Next.

4. When selecting components, expand the Qt tab followed by the Qt 5.14.2 tab. Select MSVC 2017 x64 and click next.

5. Click through the remaining prompts and then click Install.

6. Open Visual Studios 2017 and set the runtime versions to RELEASE and x64.

7. Attempt to build the code once and then copy the contents of "/LibResources/runtime bin" from the cloned repo into "/x64/Release"
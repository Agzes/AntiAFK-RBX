# How do I compile a C++ application in Visual Studio?

> [!NOTE]
> Since version **v.3.2.0**, the repository includes a Visual Studio Solution (`.sln`) file. You no longer need to manually create a project and copy code.

## Step 1: Download and install Visual Studio

1. Go to [Visual Studio official website](https://visualstudio.microsoft.com/downloads/).
2. Select the version of Visual Studio (for example, **Community**) and click **Download**.
3. Run the installer and select the workload **Development of classic applications in C++**. Make sure that you have installed the necessary components.
4. Complete the installation and open Visual Studio.

## Step 2: Get the source code

There are several ways to get the project files:

*   **Option A (Easiest):** On the GitHub repository page, click the green **Code** button and select **Open with Visual Studio**.
*   **Option B:** Clone the repository using Git: `git clone https://github.com/Agzes/AntiAFK-RBX.git`
*   **Option C:** Download the repository as a ZIP file from GitHub and extract it to a folder.

## Step 3: Open the project and Build

1. If you used **Option A**, Visual Studio will open the project automatically.
2. If you used **Option B or C**, navigate to the project folder and double-click the `AntiAFK-RBX.sln` file to open it in Visual Studio.
3. Once the project is loaded, make sure the configuration is set to **Release** and **x64** (recommended).
4. Press **Ctrl+Shift+B** (or go to **Build > Build Solution**) to compile the project.
5. After a successful build, you can press **F5** to run the application.

## Step 4: Verify files

1. After successful compilation, an icon should appear in the Windows tray (usually in the lower right corner) indicating that the application is running.
2. In Windows Explorer, navigate to the `x64/Release` (or `x64/Debug`) folder inside your project directory. You should see the compiled `AntiAFK-RBX.exe` file.


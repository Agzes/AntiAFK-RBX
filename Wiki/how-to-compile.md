# How do I compile a C++ application in Visual Studio?

## Step 1: Download and install Visual Studio

1. Go to [Visual Studio official website](https://visualstudio.microsoft.com/downloads/).
2. Select the version of Visual Studio (for example, **Community**) and click **Download**.
3. Run the installer and select the workload **Development of classic applications in C++**. Make sure that you have installed the necessary components.
4. Complete the installation and open Visual Studio.

## Step 2: Create a new project

1. Launch Visual Studio.
2. On the home screen, select **Create a new project**.
3. In the **Create a Project** window, select **An empty C++ project**.
4. Enter the name of the project (for example, `AntiAFK`) and select a location to save it. Click **Create**.
5. In the next window, you can leave the default settings and click **Create**.

## Step 3: Copy the code from the main repository file

1. Go to the AntiAFK-RBX repository, where the C++ application code is located. Open the main file `AntiAFK-RBX.cpp `. 
2. Copy all the code from this file.
3. In Visual Studio, open the file `Source.cpp ` (or `main.cpp `, depending on the template).
4. Paste the copied code into this file, replacing its contents.

## Step 4: Compile and verify files

1. Press the **Run** button (or the 'F5` key) to compile and run the application.
2. After successful compilation, an icon should appear in the Visual Studio tray (usually in the lower right corner) indicating that the application is running.
3. In Windows Explorer, navigate to your project folder. You should see the compiled `.exe` file (it is usually located in the `Debug` or `Release` folder inside your project directory).
4. You should also see the `.cpp` file (for example, `main.cpp ` or `Source.cpp `), which you edited.




##### 🤖 | made with AI there may be mistakes

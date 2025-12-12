# 📢 Generics Public Preview for .NET nanoFramework CLR

## Table of Contents

- [Generics in C# (Overview)](#generics-in-c-overview)
- [Using Preview Firmware with Generics Support](#using-preview-firmware-with-generics-support)
- [Installing the Preview Visual Studio Extension](#installing-the-preview-visual-studio-extension)
- [Updating NuGet Packages to 2.0+ Preview](#updating-nuget-packages-to-20-preview)
- [Feedback and Known Issues](#feedback-and-known-issues)

## Generics in C# (Overview)

**Generics** in C# allow you to define classes, methods, and data structures with **type parameters**, which are placeholders for specific types that are supplied when you use them ([Generic type parameters (C# Programming Guide) - Learn Microsoft](https://learn.microsoft.com/dotnet/csharp/programming-guide/generics/generic-type-parameters?WT.mc_id=DT-MVP-5004179)). This enables **type-safe** code reuse: you can write a single class or method that works for any data type while still catching type errors at compile time. For example, the .NET `List<T>` class is generic – you can create a `List<int>` for integers or a `List<string>` for strings, and the compiler ensures that only the specified type is used in that list.

Key benefits of generics include:

- **Type Safety:** They eliminate the need for casting or boxing by preserving type information.  
- **Reusability:** You can implement algorithms once, independently of specific data types, and reuse the code for different types.  
- **Performance:** They reduce runtime overhead by avoiding conversions; e.g., storing value types in a generic collection avoids boxing/unboxing.

**Example – Generic Method:** Below is a simple generic method that returns the larger of two values of any comparable type (e.g., numbers, strings). The type parameter `T` must implement `IComparable<T>` so that they can be compared:

```csharp
// Generic method to get the maximum of two values
static T Max<T>(T a, T b) where T : IComparable<T>
{
    return (a.CompareTo(b) >= 0) ? a : b;
}

// Usage:
int biggerInt = Max(10, 42);         // works with int
string laterString = Max("apple", "zebra"); // works with string
```

**Example – Generic Class:** You can also define generic classes. For instance, a minimal generic stack class could be defined as:

```csharp
public class Stack<T>
{
    private T[] _items;
    private int _count;
    public Stack(int size) => _items = new T[size];
    public void Push(T item) => _items[_count++] = item;
    public T Pop() => _items[--_count];
}
```

This `Stack<T>` can be used as `Stack<int>` for an integer stack or `Stack<string>` for a string stack, etc., each providing compile-time type enforcement.

*Generics have been a fundamental feature in desktop .NET for years. However, due to the constraints of embedded devices, previous versions of **.NET nanoFramework** did **not have support for generics**. With this preview, .NET nanoFramework is introducing support for generics, closing the gap with standard C# capabilities.* Developers can now write more flexible and reusable code on microcontrollers, just as they would on the full .NET runtime. Keep in mind that this is a public **preview** following a closed preview with intensive testing quietly happening in the background. While most generic scenarios are expected to work, there may be limitations or bugs as this feature is brand new to constrained devices.

## Using Preview Firmware with Generics Support

To use C# with generics on a nanoFramework device, you **must update the device’s firmware to a preview build** that includes generics support. This would be v2. The **nanoFramework Firmware Flasher** (`nanoff` CLI tool) provides an easy way to flash the latest preview firmware.

**Flash a Preview Firmware:** Use the `--preview` option with `nanoff` to fetch firmware from the development feed (which contains experimental or major feature updates like generics).
For example, to update an ESP32 device on COM4 to the latest preview firmware:

```bash
nanoff --target ESP32_WROOM_32 --serialport COM4 --update --preview
```

If you’re using an STM32 board or other targets, specify the appropriate `--target` name (and connection parameters if needed). The `--preview` flag directs `nanoff` to download the firmware from the **nanoFramework development feed** (Cloudsmith) rather than the stable feed. You’ll see output indicating a preview version (e.g. firmware version like `2.x.y-preview.z` being flashed). The development feed with all preview firmware packages is available here: **[nanoFramework Cloudsmith dev feed](https://cloudsmith.io/~net-nanoframework/repos/nanoframework-images-dev/packages/)**.

> **Note:** Ensure you have the latest version of the `nanoff` tool installed (`dotnet tool update -g nanoff`). You can list available preview targets with `nanoff --listtargets --preview` (optionally add `--platform esp32` or `stm32` to filter) to confirm that your board has a preview image available. If your device type doesn’t appear in the preview list, its generics-enabled firmware might not be published yet.

After flashing, you can verify the device firmware version via the Visual Studio Device Explorer (Device Capabilities) to confirm it’s running a preview image that supports generics.

## Installing the Preview Visual Studio Extension

Developing with the generics preview requires a **preview version of the .NET nanoFramework Visual Studio extension**, which contains the updated debugger and deployment tools.

**1. Download the Preview Extension:** The nanoFramework extension previews are published on the Open VSIX Gallery. Choose the version matching your Visual Studio edition:  
    - **Visual Studio 2019** – [Download .NET nanoFramework Extension (VS2019 preview)](https://www.vsixgallery.com/extension/455f2be5-bb07-451e-b351-a9faf3018dc9)  
    - **Visual Studio 2022/2026** – [Download .NET nanoFramework Extension (VS2022 preview)](https://www.vsixgallery.com/extension/bf694e17-fa5f-4877-9317-6d3664b2689a)  

These links provide the `.vsix` installer for the extension. (You can also set up the VSIX gallery feed in Visual Studio to get preview updates (instructions on [this](https://nanoframework.net/setup-visual-studio-to-access-preview-versions-feed/#:~:text=3,along%20with%20the%20following%20URL) blog post, but direct download is simplest.)

**2. Install the VSIX in Visual Studio:** After downloading the file, double-click on it and the extension installer will open. Just follow the workflow and confirm the options. Note that all Visual Studio instances have to be closed.

Once the preview extension is installed, Visual Studio will be aware of generics support for .NET nanoFramework projects. This extension update is necessary – older versions of the extension (from the official VS Marketplace) do not understand generics and would flag errors or fail to deploy projects using generics.

> **Warning:** Despite our efforts to make this possible using the VS extension preview, it’s not possible to support pre and post-generics projects with the same extension. Therefore, you have to install the preview version for generics support or a stable one to work with the current stable versions. It is not an option to install pre and post-generics extensions in VS2019 and VS2022, for example, as both use the same msbuild components for the project system and builds.

## Updating NuGet Packages to 2.0+ Preview

In addition to firmware and the VS extension, your project’s NuGet packages (nanoFramework class libraries) must be updated to **preview versions (2.0.x)** that include generics support. Core libraries have been revved to **2.0.0-preview** (or later) for this feature. These preview NuGets are distributed through the public NuGet.org feed.

**Update NuGet Packages:** In your nanoFramework project, open **Manage NuGet Packages** and enable "Include pre-release" (if applicable) to see the preview versions. Update all relevant **.NET nanoFramework** packages (such as **nanoFramework.CoreLibrary**, **System.*** assemblies, etc.) to the latest 2.x preview versions available from NuGet feed. The **CoreLibrary** (mscorlib) is especially important to update, as generics support starts there.

Not all libraries have a 2.0 preview version yet – the team is gradually publishing updated packages. If a particular NuGet has no 2.x version on the preview feed, it means that library hasn’t been updated for generics support yet. It is not possible to mix libraries pre-generics (version 1.x) with generics.

> **Tip:** After updating NuGets, clean and rebuild the solution. Make sure the device’s firmware **and** all NuGet package versions are aligned. A mismatch (e.g., preview libraries with an older firmware, or vice versa) can cause deployment errors (such as “Couldn’t find a valid native assembly required by mscorlib” or **type resolution failures**. For more details see [this](https://docs.nanoframework.net/content/faq/working-with-vs-extension.html#:~:text=This%20occurs%20when%20you%20are,blog%20post%20with%20the%20details) blog post). In short, use preview firmware *with* preview libraries, and stable firmware with stable libraries.

## Feedback and Known Issues

This generics implementation is in **public preview**, which means it’s feature-complete but not yet fully battle-tested. The nanoFramework team has done extensive internal testing, but due to the complexity of generics (especially on memory-constrained devices), you should expect some bugs or edge-case issues. Performance of generic code and the GC behaviour with generics are areas to monitor in this preview.

We **encourage you to try out generics in your projects and provide feedback**. If you encounter issues, have questions, or want to share results, please reach out on our Discord channel **`#generics-public-preview`** (join via [this invite link](https://discord.gg/DNsKbqjnvn)). This channel is dedicated to feedback on the generics preview. You can also report issues on the GitHub repository, but Discord will get you in touch with the dev team and other preview users quickly.

Known issues and limitations will be tracked as they are discovered. Some initial known items include:

- **Debugging Quirks:** In this preview, debugging generic types might have minor hiccups (e.g., watch windows not showing generic type parameters correctly). These are being addressed in upcoming extension updates.
- **Incomplete Library Support:** As noted, not every official library is updated for generics yet. You might find that using generics in certain peripheral libraries (Networking, etc.) isn’t possible until their preview is released.
- **Memory Usage:** Generics add some overhead in metadata; on very constrained devices, you might notice a slight increase in deployment size or memory usage. Keep an eye on memory allocation if your application is near the device’s limits.

Despite these issues, adding generics is a **major milestone** for .NET nanoFramework. We’re excited to bring modern C# features to embedded developers, and with your help and feedback, we’ll refine this into a rock-solid feature for the stable release. **Thank you for trying the generics preview!** Enjoy the type-safe productivity gains on your microcontroller projects, and happy coding with .NET nanoFramework.

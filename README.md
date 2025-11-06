# TFM

# 🧱 Unreal Engine 5.5 — Build & Compilation Workflow (C++ + Editor)

This document explains the professional setup implemented to **build and run the Unreal Engine project (TFM)** using Epic’s official `Build.bat` tool.  
The goal is to ensure **stable builds**, avoid **Hot Reload issues**, and maintain **consistency** between local and CI/CD environments (e.g., GitHub Actions).

---

## 🎯 Objective

Establish a professional Unreal Engine 5.5 build workflow that:

- Prevents Blueprint corruption caused by Hot Reload.  
- Generates clean, reproducible builds (Development / Shipping).  
- Allows automation from PowerShell or a CI/CD pipeline.  
- Keeps C++ changes properly synced between Visual Studio and Unreal Editor.  

---

## ⚙️ Tools Used

- **Unreal Engine 5.5**
- **Visual Studio 2022**
- **PowerShell**
- **Unreal Build Tool (UBT)** — executed via `Build.bat`
- **RunUAT.bat** — for packaging and shipping builds
- **GitHub Actions (Self-hosted runner)** — for automated builds and releases

---

## 🚀 Local Build Workflow (Using `Build.bat`)

The recommended way to build the project is through Epic’s official build script:

```powershell
& "..\UE_5.5\Engine\Build\BatchFiles\Build.bat" TFMEditor Win64 Development -project="..\TFM\TFM.uproject" -NoHotReloadFromIDE

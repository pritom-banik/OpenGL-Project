# 3D Visualization of KUET Academic Building and Surrounding Campus
A 3D interactive campus simulation developed for the **Computer Graphics and Image Processing Laboratory** course using **C++** and **OpenGL 3.3 Core Profile**.

The project visualizes the Academic Building, student residential halls, roads, roundabout, streetlights, and surrounding campus environment of **Khulna University of Engineering & Technology (KUET)** with dynamic lighting, shadows, vehicle movement, and multiple shading models.

![Campus Overview](Screen%20Shots/bg.png)

---

## Screenshots

### Campus Views & Night Mode
| Daytime Top View | Night Mode (Orbit View) |
| :---: | :---: |
| ![Top View](Screen%20Shots/topview.png) | ![Night Orbit View](Screen%20Shots/night%20orbit%20view.png) |

| Side Perspective View | Campus Bird's Eye View |
| :---: | :---: |
| ![Side View](Screen%20Shots/sideview.png) | ![Top View Cloud](Screen%20Shots/topviewcld.png) |

### Shading Models Comparison
| Flat Shading (`F`) | Gouraud Shading (`G`) | Phong Shading (`P`) |
| :---: | :---: | :---: |
| ![Flat Shading](Screen%20Shots/flat%20shading.png) | ![Gouraud Shading](Screen%20Shots/gauraud%20shading.png) | ![Phong Shading](Screen%20Shots/phong%20shading.png) |

---

## Features

- **Campus Modeling**:
  - Main Academic Building complex (4 interconnected blocks, corridors, and extensions).

  - Roads, central roundabout, sidewalks, and wide green terrain.
  - Procedural trees and  double-arm streetlamp posts with glowing bulbs.

- **Shading & Lighting**:
  - **3 Shading Models**: Flat, Gouraud (per-vertex), and Phong (per-fragment).
  - **5 Light Types**: Directional Sun/Moon, Point Light, Spotlights on lamp heads, Area Light, and night Hall-Window Area Lights.
  - **Day / Night Cycle**: Toggle day/night lighting, sky colors, and automatic streetlamp activation.
  - **Real-Time Shadow Mapping**: Dynamic shadow caster selection with $3 \times 3$ Percentage-Closer Filtering (PCF).

- **Cameras & Movement**:
  - **Orbit Camera**: Left-click drag to rotate around the campus; scroll wheel to zoom.
  - **First-Person Camera**: Ground-level view with `W`/`A`/`S`/`D` walking and right-click mouse-look.

- **Vehicles & GUI**:
  - Autonomous traffic simulation (bus and car with wheel spinning).
  - **Manual Bus Control**: Direct driving control using arrow keys or GUI buttons.
  - **Dear ImGui Dashboard**: Real-time interface to toggle lights, shadows, cameras, and shading modes.

---

## Controls Reference

| Key / Input | Action |
| :--- | :--- |
| **Left Click + Drag** | Orbit camera (rotate view) |
| **Mouse Wheel** | Zoom in / out |
| **Right Click + Drag** | Look around (First-person mode) |
| **`W` / `A` / `S` / `D`** | Walk forward / left / backward / right (First-person mode) |
| **`Tab`** | Toggle between **Orbit** and **First-Person** camera |
| **`F`** | Switch to **Flat Shading** |
| **`G`** | Switch to **Gouraud Shading** |
| **`P`** | Switch to **Phong Shading** |
| **`L`** | Toggle **Street Lamp Spotlights** and bulb glow |
| **`H`** | Toggle **Hall Window Lights** (available at night) |
| **`Up` / `Down` Arrow** | Drive bus forward / reverse (Manual bus mode) |
| **`Left` / `Right` Arrow** | Steer bus left / right (Manual bus mode) |
| **`Esc`** | Exit application |

---

## How to Build and Run

1. **Open Project**: Open `Graphics.slnx` (or `Graphics.vcxproj`) in **Visual Studio 2022**.
2. **Configuration**: Select **Release** (or **Debug**) and **x64** platform.
3. **Build & Run**: Press `Ctrl + F5` to run without debugging.
   > **Note**: Make sure the working directory is set to the project root so shaders (`default.vert`, `default.frag`) and textures (`road-texture.png`, `seamless_brick.png`) can be loaded.

---

## Acknowledgements & Learning Resources

I would like to express my sincere gratitude to my course teachers and lab instructors of the **Computer Graphics and Image Processing Laboratory** course.

Special thanks to **Victor Gordan** for his outstanding OpenGL tutorial playlist:
- **YouTube Playlist**: [OpenGL 3.3 / C++ Tutorial Series by Victor Gordan](https://youtube.com/playlist?list=PLPaoO-vpZnumdcb4tZc4x5Q-v7CkrQ6M-&si=r6GGtD2MxvhC38gF)

The foundations of OpenGL buffer management (`VAO`, `VBO`, `EBO`), shader architecture, textures, and camera implementations in this project were learned from his tutorials.

---

## Formal Statement of AI Assistance

In adherence to academic integrity and transparency guidelines, I formally acknowledge the use of artificial intelligence tools during this project:

- **GitHub Copilot**: Used for code completion and boilerplate assistance during C++ programming.
- **Google Gemini**: Used for debugging shader uniform bindings, lighting issues, and problem-solving.
- **Claude**: Used for assistance in structuring and generating the project laboratory report.

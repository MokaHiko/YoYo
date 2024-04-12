<!-- Improved compatibility of back to top link: See: https://github.com/othneildrew/Best-README-Template/pull/73 -->
<a name="readme-top"></a>
<!--
*** Thanks for checking out the Best-README-Template. If you have a suggestion
*** that would make this better, please fork the repo and create a pull request
*** or simply open an issue with the tag "enhancement".
*** Don't forget to give the project a star!
*** Thanks again! Now go create something AMAZING! :D
-->



<!-- PROJECT SHIELDS -->
<!--
*** I'm using markdown "reference style" links for readability.
*** Reference links are enclosed in brackets [ ] instead of parentheses ( ).
*** See the bottom of this document for the declaration of the reference variables
*** for contributors-url, forks-url, etc. This is an optional, concise syntax you may use.
*** https://www.markdownguide.org/basic-syntax/#reference-style-links
-->
[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]
[![LinkedIn][linkedin-shield]][linkedin-url]

<!-- PROJECT LOGO -->
<br />
<div align="center">
  <a href="https://github.com/MokaHiko/YoYo">
    <img src="branding/logo.jpg" alt="Logo" width="200" height="200">
  </a>

<h3 align="center">YoYo</h3>

  <p align="center">
    lightweight cross platform 3d renderer
    <br />
    <a href="https://github.com/MokaHiko/YoYo"><strong>Explore the docs »</strong></a>
    <br />
    <br />
    <a href="https://github.com/MokaHiko/YoYo">View Demo</a>
    ·
    <a href="https://github.com/MokaHiko/YoYo/issues">Report Bug</a>
    ·
    <a href="https://github.com/MokaHiko/YoYo/issues">Request Feature</a>
  </p>
</div>



<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
      <!-- <ul>
        <li><a href="#built-with">Built With</a></li>
      </ul> -->
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
  </ol>
</details>

<!-- ABOUT THE PROJECT -->
## About The Project

[![Product Name Screen Shot][product-screenshot]](./)

Welcome to YoYo, a lightweight and versatile 3D renderer designed for simplicity and performance. YoYo is crafted with the vision of providing developers with a powerful toolset for creating 3D graphics applications with ease.

<p align="right">(<a href="#readme-top">back to top</a>)</p>
<!-- GETTING STARTED -->

### Getting Started

This is an example of how you may give instructions on setting up your project locally.
To get a local copy up and running follow these simple example steps.

### Prerequisites

* CMake https://cmake.org/download/

* The Vulkan SDK must be an environment variable %VULKAN_SDK%
  ```sh
  echo %VULKAN_SDK%
  C:\Program Files (x86)\Vulkan 
  ```
<p align="right">(<a href="#readme-top">back to top</a>)</p>

### Installation

1. Clone the repo
   ```sh
   git clone --recursive https://github.com/MokaHiko/YoYoQuickStart.git.
   ```
2. 
   If the repository was cloned non-recursively previously, use git submodule update --init to clone the necessary submodules.
   ```sh
    git submodule update --init 
   ```
3. Run `CMake` in root
   ```js
   cmake -S ./ -B ./build
   ```
<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- CONTRIBUTING -->
## Contributing

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement".
Don't forget to give the project a star! Thanks again!

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

See the [open issues](https://github.com/MokaHiko/YoYo/issues) for a full list of proposed features (and known issues).

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- LICENSE -->
## License

Distributed under the MIT License. See `LICENSE.txt` for more information.

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- CONTACT -->
## Contact

Your Name - [@HikoMoka](https://twitter.com/HikoMoka) - cellscommander@gmail.com

Project Link: [https://github.com/MokaHiko/YoYo](https://github.com/MokaHiko/YoYo)

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/MokaHiko/YoYo.svg?style=for-the-badge
[contributors-url]: https://github.com/MokaHiko/YoYo/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/MokaHiko/YoYo.svg?style=for-the-badge
[forks-url]: https://github.com/MokaHiko/YoYo/network/members
[stars-shield]: https://img.shields.io/github/stars/MokaHiko/YoYo.svg?style=for-the-badge
[stars-url]: https://github.com/MokaHiko/YoYo/stargazers
[issues-shield]: https://img.shields.io/github/issues/MokaHiko/YoYo.svg?style=for-the-badge
[issues-url]: https://github.com/MokaHiko/YoYo/issues
[license-shield]: https://img.shields.io/github/license/MokaHiko/YoYo.svg?style=for-the-badge
[license-url]: https://github.com/MokaHiko/YoYo/blob/master/LICENSE.txt
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=for-the-badge&logo=linkedin&colorB=555
[linkedin-url]: https://www.linkedin.com/in/christiansolon/
[product-screenshot]: branding/sun_test.gif
[Cpp]: https://img.shields.io/badge/Cpp-000000?style=for-the-badge&logo=nextdotjs&logoColor=white
[Next-url]: https://nextjs.org/
[Vulkan.js]: https://img.shields.io/badge/Vulkan-20232A?style=for-the-badge&logo=Vulkan&logoColor=61DAFB
[Vulkan-url]: https://www.vulkan.org/

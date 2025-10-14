FROM mcr.microsoft.com/devcontainers/base:ubuntu-24.04

# Install base building dependencies
RUN apt-get update &&                                                          \
    apt-get install -y                                                         \
      build-essential                                                          \
      curl                                                                     \
      git                                                                      \
      mingw-w64                                                                \
      wget                                                                     \
      unzip                                                                    \
      &&                                                                       \
    rm -rf /var/lib/apt/lists/*

# Install CMake
ARG CMAKE_VERSION=4.1.2
RUN mkdir -p /opt/local/cmake &&                                               \
    curl -fsSL -o install-script.sh                                            \
      https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-linux-$(uname -i).sh && \
    bash install-script.sh --skip-license --prefix=/opt/local/cmake &&         \
    rm -f install-script.sh
ENV PATH="/opt/local/cmake/bin:${PATH}"

# Install Ninja
ARG NINJA_VERSION=1.13.1
RUN mkdir -p /opt/local/ninja/bin &&                                           \
    if [ "$(uname -i)" = "aarch64" ]; then                                     \
      export archive=ninja-linux-aarch64;                                      \
    else                                                                       \
      export archive=ninja-linux;                                              \
    fi &&                                                                      \
    curl -fsSL -o "${archive}.zip"                                             \
      https://github.com/ninja-build/ninja/releases/download/v${NINJA_VERSION}/${archive}.zip && \
    unzip "${archive}.zip" -d /opt/local/ninja/bin &&                          \
    chmod +x /opt/local/ninja/bin/ninja &&                                     \
    rm -f "${archive}.zip"
ENV PATH="/opt/local/ninja/bin:${PATH}"

# Install LLVM
ARG LLVM_VERSION=20
RUN apt-get update &&                                                          \
    apt-get install -y                                                         \
      clang-${LLVM_VERSION}                                                    \
      clang-tools-${LLVM_VERSION}                                              \
      clang-format-${LLVM_VERSION}                                             \
      clang-tidy-${LLVM_VERSION}                                               \
      clangd-${LLVM_VERSION}                                                   \
      llvm-${LLVM_VERSION}                                                     \
      lldb-${LLVM_VERSION}                                                     \
      lld-${LLVM_VERSION}                                                      \
      &&                                                                       \
    rm -rf /var/lib/apt/lists/* &&                                             \
    export llvm_dir=/opt/local/llvm-${LLVM_VERSION}/bin &&                     \
    mkdir -p "${llvm_dir}" &&                                                  \
    for tool in clang clang++ clangd clang-tidy clang-format lldb lld; do      \
      ln -is "/usr/bin/${tool}-${LLVM_VERSION}" "${llvm_dir}/${tool}";         \
    done &&                                                                    \
    for versioned_tool in $(find /usr/bin -name 'llvm-*'); do                  \
      tool_name=$(basename "${versioned_tool}");                               \
      tool_name=$(echo "${tool_name}" | sed -E "s@-${LLVM_VERSION}@@");        \
      ln -s "${versioned_tool}" "${llvm_dir}/${tool_name}";                    \
    done
ENV PATH="/opt/local/llvm-${LLVM_VERSION}/bin:${PATH}"
ENV CC="/opt/local/llvm-${LLVM_VERSION}/bin/clang"
ENV CXX="/opt/local/llvm-${LLVM_VERSION}/bin/clang++"

# Install SDL2, OpenGL, GLEW, and GLFW
RUN apt update &&                                                              \
    apt install -y                                                             \
      libglew-dev                                                              \
      libgl-dev                                                                \
      libglfw3-dev                                                             \
      libsdl2-dev                                                              \
      libsdl2-2.0-0 &&                                                         \
    rm -rf /var/lib/apt/lists/*

# Install Vulkan SDK
# LunarG doesn't provide ARM builds, so we need to use a community prebuilt
# version for aarch64.
ARG VULKAN_SDK=1.4.328.1
RUN mkdir -p /opt/local/vulkan &&                                              \
    if [ "$(uname -i)" = "aarch64" ]; then                                     \
      url=https://github.com/jakoch/vulkan-sdk-arm/releases/download/${VULKAN_SDK}/vulkansdk-ubuntu-24.04-arm-${VULKAN_SDK}.tar.xz; \
    else                                                                       \
      url=https://sdk.lunarg.com/sdk/download/${VULKAN_SDK}/linux/vulkansdk-linux-x86_64-${VULKAN_SDK}.tar.xz; \
    fi &&                                                                      \
    curl -fsSL -o vulkansdk.tar.xz "${url}" &&                                 \
    tar -xf vulkansdk.tar.xz -C /opt/local/vulkan --strip-components=1 &&      \
    rm -f vulkansdk.tar.xz
RUN mkdir -p /opt/local/runtime &&                                             \
    entrypoint_path=/opt/local/runtime/entrypoint.sh &&                        \
    echo "#!/usr/bin/env bash" >> "${entrypoint_path}" &&                      \
    echo "set -e" >> "${entrypoint_path}" &&                                   \
    echo "" >> "${entrypoint_path}" &&                                         \
    echo "source /opt/local/vulkan/setup-env.sh" >> "${entrypoint_path}" &&    \
    echo "" >> "${entrypoint_path}" &&                                         \
    echo "bash" >> "${entrypoint_path}" &&                                     \
    chmod +x /opt/local/runtime/entrypoint.sh

ENTRYPOINT [ "/opt/local/runtime/entrypoint.sh" ]

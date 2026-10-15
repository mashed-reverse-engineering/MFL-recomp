# Wine + virtual X server + software GL + null audio (32 bit, MFL.exe is a 32 bit D3D9 game) for headless runs; see ghidra/README.md "Running the game".
FROM tianon/wine:latest
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
    xvfb x11-utils x11-apps xdotool imagemagick gdb procps \
    libgl1:i386 libegl1:i386 libglx-mesa0:i386 libgl1-mesa-dri:i386 libvulkan1:i386 mesa-vulkan-drivers:i386 \
    libgl1 libegl1 libgl1-mesa-dri \
    pulseaudio libpulse0 libpulse0:i386 \
    gstreamer1.0-plugins-base:i386 gstreamer1.0-plugins-good:i386 gstreamer1.0-plugins-ugly:i386 gstreamer1.0-libav:i386 \
    gstreamer1.0-plugins-base gstreamer1.0-plugins-good gstreamer1.0-plugins-ugly gstreamer1.0-libav gstreamer1.0-plugins-bad \
    && rm -rf /var/lib/apt/lists/*

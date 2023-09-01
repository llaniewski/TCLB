FROM mcr.microsoft.com/devcontainers/cpp:1-debian-11

ARG REINSTALL_CMAKE_VERSION_FROM_SOURCE="none"

# [Optional] Uncomment this section to install additional packages.
# RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
#     && apt-get -y install --no-install-recommends <your-package-list-here>

RUN wget https://raw.githubusercontent.com/CFD-GO/TCLB/master/tools/install.sh && chmod +x install.sh
RUN ./install.sh openmpi
RUN ./install.sh r
RUN ./install.sh rdep


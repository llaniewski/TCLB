FROM mcr.microsoft.com/devcontainers/cpp:1-debian-11

# [Optional] Uncomment this section to install additional packages.
# RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
#     && apt-get -y install --no-install-recommends <your-package-list-here>

COPY install.sh /tmp/
RUN chmod +x /tmp/install.sh
USER vscode
RUN sudo /tmp/install.sh openmpi
RUN sudo /tmp/install.sh r
RUN sudo /tmp/install.sh rdep

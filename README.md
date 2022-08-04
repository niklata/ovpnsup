# ovpnsup

Copyright (c) 2015-2017 Nicholas J. Kain

## Introduction

Supervises an OpenVPN process and performs actions as root when the
OpenVPN process exits.

## Rationale

It is meant to be used with an unprivileged and chroot'ed OpenVPN session.
OpenVPN will run the `up` script before dropping root and chroot'ing
the program.  However, the `down` and `ipchange` scripts will be mostly
useless as they will run within the chroot.

One poor method of working around this issue would be to have a suid root
helper program that OpenVPN could run from the down or ipchange hook.
This approach is recommended at various places around the internet,
but it is a bad idea because suid root binaries have historically
been vulnerable to many attacks (`LD_PRELOAD` and similar are the most
well-known) and are intrinsically difficult to use safely.

Another, better method would be to listen to Netlink events and react
to changes made to the associated tunnel device.  The disadvantages here
are the complexity involved and lack of portability to non-Linux systems.

ovpnsup instead draws inspiration from process supervisors and takes
advantage of the fact that a parent process will be notified via `SIGCHLD`
when its child processes terminate.  ovpnsup is meant to act as a shim
between openvpn and a process supervisor like runit, s6, or daemontools,
so it does not attempt to keep the child openvpn process running after
termination -- it relies on an actual supervisor and just acts as a shim
that does work after the openvpn child terminates.

## Usage

The openvpn session should of course be set to stay in the foreground (no
`--daemon`) and exit on error or connection state change (`--ping-exit`).

Configuration of ovpnsup should be done at compile time by editing the
source code, although it does have a facility to use a tag argument to
identify the particular tunnel in use.  This tag is useful for systems
that have multiple tunnels.

In the default configuration, invocation should be similar to:

`ovpnsup TAG /usr/bin/openvpn --config /etc/openvpn/blah.conf`

Refer to the source code for more details.

## Downloads

* [GitLab](https://gitlab.com/niklata/ovpnsup)
* [Codeberg](https://codeberg.org/niklata/ovpnsup)
* [BitBucket](https://bitbucket.com/niklata/ovpnsup)
* [GitHub](https://github.com/niklata/ovpnsup)

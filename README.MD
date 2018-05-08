# self-updater

Self-updater demonstration written in C++. Includes the loader, client, server 
and signing tools to securely deliver automatic updates for Win32 desktop applications.

### Background

Native Win32 desktop applications don't have the luxury of a system
integrated package manager for handling automatic updates. This framework
provides a working demonstration of how to securely and efficiently deliver
and process updates.

### Security

A self-updating system should satisfy integrity and authenticity security
goals. This framework uses 4096-bit RSA keys to to sign and verify
the SHA-256 signature for the update data. The public is key is embedded
into the client binary for verification. Assuming the initial delivery of
the software is done over a trusted source, the only valid issuers
for updates are those with access to the private key.

Update data is transmitted over UDP. The primary factor in this decision
was reducing server memory overhead from TCP connections (with reduced CPU
load as a bonus). Expected memory overhead is roughly:

```
(Update File Size) + (Max Connections) * 80 bytes
```

The update data is stored directly in memory to avoid accessing the disk. The
client and server verify ownership of the IP by exchanging 64-bit cryptographic
tokens. Each connection is assigned a limited amount of network bandwidth
before they are throttled.  Each IP address is limited to a maximum number of 
concurrent connections. These features mitigate a number of common network
attacks:

- IP spoofing for both client/server are prevented (can't pass token test)
- Denial of service from a single connection is limited
- Amplification attacks using the protocol are ineffective

Man-in-the-middle attack prevention relies on the authenticity guarantees
from the update system.

No heavy cryptographic functions need to run on the server (like in HTTPS) since
confidentiality isn't a security goal for software updates -- the update data isn't sensitive.

### Integration and Scalability

No efforts are made on the client side to limit download speed. Integration
into your application should make efforts to dynamically adjust the
send rate based on dropped packets and connection latency.

The demo server architecture runs on a single port/single thread. 
Multiple server instances could be run per thread on different ports or 
different systems (with the negotiation for which occurring at a load 
balancer which the client would connect to first (for large scale update delivery).

Update data is sent for a single file as-is. If you need support for 
multiple file patching, delta-updates or compression you should deliver 
an executable with the utilities and data to handle these features (and decouple
it from the update system itself).

The security guarantees only hold as long as the private key remains secure.
No key revocation method is bundled with the framework (as it would require 
external verification and trust from another provider).

### Building

1. Install [Visual Studio](https://www.visualstudio.com/).
2. Open `updater.sln`.
3. Change solution configuration to `Release`.
4. Build the  solution.

### Setup

1. Copy and rename `client.dll` to `update.dll`
1. Run `signtool.exe` to generate `pub.h`, `pub.key`, `pri.key` and `update.sig`.
3. Replace `public_key` data in `client_updater.cpp` with the contents from the generated `pub.h`.
4. Rebuild solution.
5. Start `server.exe` and `loader.exe` to demonstrate the update.

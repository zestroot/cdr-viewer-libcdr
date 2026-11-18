# libcdr and librevenge for CDR Viewer

ZestRoot's CDR Viewer, an Android app that opens CorelDRAW drawings, has two libraries built into its native library (`libcdrcore.so`) that are under the Mozilla Public License 2.0:

- **libcdr**, from LibreOffice, which reads CorelDRAW files. We changed it, and the changes are here.
- **librevenge**, which libcdr draws through. We didn't change it.

This repository holds the source of both, exactly as the app builds them, as section 3.2 of the MPL asks. It's offered under the MPL 2.0, at no charge.

## What's here

| Folder | What it is |
|---|---|
| `libcdr/` | libcdr at upstream commit [`eb0a026`](https://github.com/LibreOffice/libcdr/commit/eb0a026e89d129919c32f5412d72c794c0d1f20f) (a `git archive` of it), with our five changes on top |
| `librevenge-0.0.5/` | the librevenge 0.0.5 release tarball, unpacked as is (`librevenge-0.0.5.tar.bz2`, sha256 `5892ca6796f7a2a93d580832e907e849b19d980b40d326a283b18877ab6de0c5`) |

The first commit is the upstream source. Each later commit is one of our changes to libcdr, so `git log -p` shows each one:

1. **Master layers (X6 and later).** A master layer (a background, a frame, a calendar's grid) was drawn on no page. Each page now draws the master layers it lists, in its own stacking order.
2. **PowerClips (X6 and later).** Their contents were dropped. They now draw clipped to the container's outline. Master layers and PowerClip contents are also placed right on a page whose size differs from the master page's.
3. **A stop check.** A per-thread check in `CDRInternalStream` lets the app cancel a file, or stop at a memory limit, part way through reading it.
4. **Transparency (X3 and later).** The amount was read from the wrong bytes, so most transparent objects were invisible. It's now read from the record.
5. **Pictures read in place.** libcdr puts a key beside each picture it hands over, so the app reads the bytes where they are instead of decoding base64. The output is unchanged.

## Licences

- **libcdr:** MPL 2.0 (`libcdr/COPYING.MPL`). Our changes are under the MPL 2.0 too.
- **librevenge:** MPL 2.0 or LGPL 2.1 or later, at your choice (`librevenge-0.0.5/COPYING.*`). The app uses it under the MPL 2.0.
- `LICENSE` at the top is a copy of the MPL 2.0.

The app's own code (the Flutter app and the `cdr-core` library that calls libcdr) isn't here: it's separate work under its own terms, which the MPL allows.

## How the app builds them

Both are built as static libraries with autotools, then linked into `libcdrcore.so` with the Android NDK at API 28, for `arm64-v8a`, `armeabi-v7a` and `x86_64`.

- **librevenge:** `./configure --disable-shared --enable-static --disable-werror --disable-tests --without-docs`.
- **libcdr:** `./autogen.sh`, then `./configure --disable-shared --enable-static --without-icu --disable-werror --disable-tests --without-docs`, then `make -C src/lib`, `make -C src/lib install` and `make -C inc install` (only the library, not libcdr's tools).
  - It's compiled with `-Diconv_open=cdrcore_iconv_open -Diconv=cdrcore_iconv -Diconv_close=cdrcore_iconv_close`. Android's iconv lacks the Windows code pages libcdr needs, so the app supplies these three functions itself.
- **Flags:** `-O2 -fPIC -ffunction-sections -fdata-sections -fvisibility=hidden` for both.
- **Other inputs:** Little CMS 2.16 (MIT), Boost 1.83 headers (Boost Software License 1.0), and zlib from the NDK.

## Other libraries in the app

Little CMS 2.16 (MIT) and the Boost 1.83 headers (BSL 1.0) are also built in. Their licences don't ask for their source to be offered, so it isn't here. The app lists every licence on its open-source licences screen.

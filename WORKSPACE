load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive", "http_file")

release = "1.10.0"
http_archive(
  name = "googletest",
  urls = ["https://github.com/google/googletest/archive/release-" + release + ".tar.gz"],
  strip_prefix = "googletest-release-" + release,
)

http_file(
  name = "cpplint_build",
  urls = ["https://raw.githubusercontent.com/nicmcd/pkgbuild/master/cpplint.BUILD"],
)

release = "1.5.4"
http_archive(
  name = "cpplint",
  urls = ["https://github.com/cpplint/cpplint/archive/" + release + ".tar.gz"],
  strip_prefix = "cpplint-" + release,
  build_file = "@cpplint_build//file:downloaded",
)

http_file(
  name = "zlib_build",
  urls = ["https://raw.githubusercontent.com/nicmcd/pkgbuild/master/zlib.BUILD"],
)

version = "1.2.11"
http_archive(
  name = "zlib",
  urls = ["https://www.zlib.net/zlib-" + version + ".tar.gz"],
  strip_prefix = "zlib-" + version,
  build_file = "@zlib_build//file:downloaded",
)

hash = "6bdfaba"
http_archive(
  name = "boringssl",
  urls = ["https://github.com/google/boringssl/tarball/" + hash],
  type = "tar.gz",
  strip_prefix = "google-boringssl-" + hash,
)

hash = "2b65641"
http_archive(
  name = "libprim",
  urls = ["https://github.com/nicmcd/libprim/tarball/" + hash],
  type = "tar.gz",
  strip_prefix = "nicmcd-libprim-" + hash,
)

hash = "cc7075f"
http_archive(
  name = "librnd",
  urls = ["https://github.com/nicmcd/librnd/tarball/" + hash],
  type = "tar.gz",
  strip_prefix = "nicmcd-librnd-" + hash,
)

hash = "2b5419b"
http_archive(
  name = "libdes",
  urls = ["https://github.com/nicmcd/libdes/tarball/" + hash],
  type = "tar.gz",
  strip_prefix = "nicmcd-libdes-" + hash,
)

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
  name = "clang_format",
  urls = ["https://raw.githubusercontent.com/nicmcd/pkgbuild/master/clang-format"],
)

http_file(
    name = "nlohmann_json_build",
    urls = ["https://raw.githubusercontent.com/nicmcd/pkgbuild/master/nlohmannjson.BUILD"],
)

release = "3.9.1"
http_archive(
    name = "nlohmann_json",
    urls = ["https://github.com/nlohmann/json/archive/v" + release + ".tar.gz"],
    strip_prefix = "json-" + release,
    build_file = "@nlohmann_json_build//file:downloaded",
)

http_file(
    name = "numa_build",
    urls = ["https://raw.githubusercontent.com/nicmcd/pkgbuild/master/numa.BUILD"],
)

http_file(
    name = "numa_patch",
    urls = ["https://raw.githubusercontent.com/nicmcd/pkgbuild/master/numactl_v2.0.14.patch"],
)

release = "2.0.14"
http_archive(
    name = "numactl",
    urls = ["https://github.com/numactl/numactl/archive/refs/tags/v" + release + ".tar.gz"],
    strip_prefix = "numactl-" + release,
    build_file = "@numa_build//file:downloaded",
    patches = ["@numa_patch//file:downloaded"],
    patch_args = ["-p1"],
    patch_cmds = ["./autogen.sh", "./configure"],
)

http_file(
  name = "zlib_build",
  urls = ["https://raw.githubusercontent.com/nicmcd/pkgbuild/master/zlib.BUILD"],
)

version = "1.2.12"
http_archive(
  name = "zlib",
  urls = ["https://www.zlib.net/zlib-" + version + ".tar.gz"],
  strip_prefix = "zlib-" + version,
  build_file = "@zlib_build//file:downloaded",
)

hash = "f564c5c"
http_archive(
  name = "libprim",
  urls = ["https://github.com/nicmcd/libprim/tarball/" + hash],
  type = "tar.gz",
  strip_prefix = "nicmcd-libprim-" + hash,
)

hash = "5f52dc7"
http_archive(
  name = "libfactory",
  urls = ["https://github.com/nicmcd/libfactory/tarball/" + hash],
  type = "tar.gz",
  strip_prefix = "nicmcd-libfactory-" + hash,
)

hash = "9820e0f"
http_archive(
  name = "librnd",
  urls = ["https://github.com/nicmcd/librnd/tarball/" + hash],
  type = "tar.gz",
  strip_prefix = "nicmcd-librnd-" + hash,
)

hash = "6570b57"
http_archive(
  name = "libfio",
  urls = ["https://github.com/nicmcd/libfio/tarball/" + hash],
  type = "tar.gz",
  strip_prefix = "nicmcd-libfio-" + hash,
)

hash = "ee4a54f"
http_archive(
  name = "libstrop",
  urls = ["https://github.com/nicmcd/libstrop/tarball/" + hash],
  type = "tar.gz",
  strip_prefix = "nicmcd-libstrop-" + hash,
)

hash = "264f2b1"
http_archive(
  name = "libsettings",
  urls = ["https://github.com/nicmcd/libsettings/tarball/" + hash],
  type = "tar.gz",
  strip_prefix = "nicmcd-libsettings-" + hash,
)

hash = "f36c54e"
http_archive(
  name = "libdes",
  urls = ["https://github.com/nicmcd/libdes/tarball/" + hash],
  type = "tar.gz",
  strip_prefix = "nicmcd-libdes-" + hash,
)

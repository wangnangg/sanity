#!/usr/bin/python3
import os.path


def list2makestr(objs):
    obj_str = []
    for obj in objs:
        obj_str.append(obj)
        obj_str.append(' ')
    return ''.join(obj_str)


# recursively list all files that match specified extensions
def find_files(target_dir, exts):
    if type(exts) is str:
        exts = [exts]
    files = []
    for dirpath, dirnames, filenames in os.walk(target_dir):
        for f in filenames:
            for e in exts:
                if (f.endswith(e)):
                    files.append(os.path.join(dirpath, f))
                    break

    return files


target_set = set()


def dir_rule(target):
    if target in target_set:
        return ''
    else:
        target_set.add(target)
        return "%s:\n\tmkdir -p $@\n" % target


def compile_rule(compiler, flags, src_file, target):
    if target in target_set:
        return ''
    else:
        target_set.add(target)
        target_path = os.path.dirname(target)
        return "%s: %s | %s \n\t%s %s -c %s -o %s\n" % (
            target, src_file, target_path, compiler, flags, src_file,
            target) + dir_rule(target_path)


def link_rule(linker, flags, objs, target):
    if target in target_set:
        return ''
    else:
        target_set.add(target)
        target_path = os.path.dirname(target)
        objs_str = list2makestr(objs)
        return "%s: %s | %s\n\t%s %s %s -o %s\n" % (
            target, objs_str, target_path, linker, objs_str, flags,
            target) + dir_rule(target_path)


def ar_rule(ar, flags, objs, target):
    if target in target_set:
        return ''
    else:
        target_set.add(target)
        target_path = os.path.dirname(target)
        objs_str = list2makestr(objs)
        return "%s: %s | %s\n\t%s %s %s %s\n" % (
            target, objs_str, target_path, ar, flags, target,
            objs_str) + dir_rule(target_path)


def change_ext(filename, new_ext):
    return os.path.splitext(filename)[0] + new_ext


def transform_src_files(source_files, obj_dir):
    objs = []
    for src in source_files:
        obj = os.path.join(obj_dir, src)
        obj = change_ext(obj, '.o')
        objs.append(obj)
    return objs


cpp_compiler = '${cpp_compiler}'
linker = '${linker}'
makefile_body = []

src_files = find_files('src', '.cpp')
obj_files = transform_src_files(src_files, "${build_dir}")
for i in range(0, len(src_files)):
    makefile_body.append(
        compile_rule(cpp_compiler, "${flags}", src_files[i], obj_files[i]))

test_src_files = find_files('test', 'cpp')
test_obj_files = transform_src_files(test_src_files, "${build_dir}")
for i in range(0, len(test_src_files)):
    makefile_body.append(
        compile_rule(cpp_compiler, "${gtest_flags} ${flags}",
                     test_src_files[i], test_obj_files[i]))

exp_src_files = find_files('experiment', 'cpp')
exp_obj_files = transform_src_files(exp_src_files, "${build_dir}")
for i in range(0, len(exp_src_files)):
    makefile_body.append(
        compile_rule(cpp_compiler, "${gtest_flags} ${flags}",
                     exp_src_files[i], exp_obj_files[i]))

gtest_src_files = ["${gtest_dir}/src/gtest-all.cc"]
gtest_obj_files = transform_src_files(gtest_src_files, "${build_dir}")
for i in range(0, len(gtest_src_files)):
    makefile_body.append(
        compile_rule(cpp_compiler, "${gtest_flags}", gtest_src_files[i],
                     gtest_obj_files[i]))

makefile_body.append(
    link_rule(linker, "${link_flags}",
              obj_files + test_obj_files + gtest_obj_files,
              '${build_dir}/utest'))

makefile_body.append(
    link_rule(linker, "${link_flags}",
              obj_files + exp_obj_files + gtest_obj_files,
              '${build_dir}/exp'))

makefile_body.append(
    ar_rule('ar', 'crf', obj_files, "${build_dir}/libsanity.a"))

deps = list2makestr(
    map(lambda obj: change_ext(obj, '.d'),
        obj_files + test_obj_files + gtest_obj_files))

makefile_body.append('deps:=' + deps + '\n')


with open('makefile.in', 'r') as fin:
    makefilein_content = fin.read()
    with open('makefile', 'w') as fmake:
        fmake.write(makefilein_content)
        fmake.write(''.join(makefile_body))
print('build with: make config=debug|release|profile|coverage')

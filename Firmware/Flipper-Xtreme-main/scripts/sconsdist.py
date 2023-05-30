#!/usr/bin/env python3

from flipper.app import App
from os.path import join, exists, relpath
from os import makedirs, walk, environ
from update import Main as UpdateMain
import shutil
import zipfile
import tarfile
from ansi.color import fg


class ProjectDir:
    def __init__(self, project_dir):
        self.dir = project_dir
        parts = project_dir.split("-")
        self.target = parts[0]
        self.project = parts[1]
        self.flavor = parts[2] if len(parts) > 2 else ""


class Main(App):
    DIST_FILE_PREFIX = "flipper-z-"
    DIST_FOLDER_MAX_NAME_LENGTH = 80

    def init(self):
        self.subparsers = self.parser.add_subparsers(help="sub-command help")

        self.parser_copy = self.subparsers.add_parser(
            "copy", help="Copy firmware binaries & metadata"
        )

        self.parser_copy.add_argument("-p", dest="project", nargs="+", required=True)
        self.parser_copy.add_argument("-s", dest="suffix", required=True)
        self.parser_copy.add_argument("-r", dest="resources", required=False)
        self.parser_copy.add_argument(
            "--bundlever",
            dest="version",
            help="If set, bundle update package for self-update",
            required=False,
        )
        self.parser_copy.add_argument(
            "--noclean",
            dest="noclean",
            action="store_true",
            help="Don't clean output directory",
            required=False,
        )
        self.parser_copy.set_defaults(func=self.copy)

    def get_project_file_name(self, project: ProjectDir, filetype: str) -> str:
        #  Temporary fix
        project_name = project.project
        if project_name == "firmware" and filetype != "elf":
            project_name = "full"

        return self.get_dist_file_name(project_name, filetype)

    def get_dist_file_name(self, dist_artifact_type: str, filetype: str) -> str:
        return f"{self.DIST_FILE_PREFIX}{self.target}-{dist_artifact_type}-{self.args.suffix}.{filetype}"

    def get_dist_file_path(self, filename: str) -> str:
        return join(self.output_dir_path, filename)

    def copy_single_project(self, project: ProjectDir) -> None:
        obj_directory = join("build", project.dir)

        for filetype in ("elf", "bin", "dfu", "json"):
            if exists(src_file := join(obj_directory, f"{project.project}.{filetype}")):
                shutil.copyfile(
                    src_file,
                    self.get_dist_file_path(
                        self.get_project_file_name(project, filetype)
                    ),
                )
        for foldertype in ("sdk", "lib"):
            if exists(sdk_folder := join(obj_directory, foldertype)):
                self.package_zip(foldertype, sdk_folder)

    def package_zip(self, foldertype, sdk_folder):
        with zipfile.ZipFile(
            self.get_dist_file_path(self.get_dist_file_name(foldertype, "zip")),
            "w",
            zipfile.ZIP_DEFLATED,
        ) as zf:
            for root, _, files in walk(sdk_folder):
                for file in files:
                    zf.write(
                        join(root, file),
                        relpath(
                            join(root, file),
                            sdk_folder,
                        ),
                    )

    def copy(self) -> int:
        self.projects = dict(
            map(
                lambda pd: (pd.project, pd),
                map(ProjectDir, self.args.project),
            )
        )

        project_targets = set(map(lambda p: p.target, self.projects.values()))
        if len(project_targets) != 1:
            self.logger.error(f"Cannot mix targets {project_targets}!")
            return 1
        self.target = project_targets.pop()

        project_flavors = set(map(lambda p: p.flavor, self.projects.values()))
        if len(project_flavors) != 1:
            self.logger.error(f"Cannot mix flavors {project_flavors}!")
            return 2
        self.flavor = project_flavors.pop()

        dist_dir_components = [self.target]
        if self.flavor:
            dist_dir_components.append(self.flavor)

        self.output_dir_path = join("dist", "-".join(dist_dir_components))
        if exists(self.output_dir_path) and not self.args.noclean:
            try:
                shutil.rmtree(self.output_dir_path)
            except Exception as ex:
                pass

        if not exists(self.output_dir_path):
            makedirs(self.output_dir_path)

        for project in self.projects.values():
            self.copy_single_project(project)

        self.logger.info(
            fg.boldgreen(
                f"Firmware binaries can be found at:\n\t{self.output_dir_path}"
            )
        )

        if self.args.version:
            bundle_dir_name = f"{self.target}-update-{self.args.suffix}"[
                : self.DIST_FOLDER_MAX_NAME_LENGTH
            ]
            bundle_dir = join(self.output_dir_path, bundle_dir_name)
            bundle_args = [
                "generate",
                "-d",
                bundle_dir,
                "-v",
                self.args.version,
                "-t",
                self.target,
                "--dfu",
                self.get_dist_file_path(
                    self.get_project_file_name(self.projects["firmware"], "dfu")
                ),
                "--stage",
                self.get_dist_file_path(
                    self.get_project_file_name(self.projects["updater"], "bin")
                ),
            ]
            if self.args.resources:
                bundle_args.extend(
                    (
                        "-r",
                        self.args.resources,
                    )
                )
            bundle_args.extend(self.other_args)
            log_custom_fz_name = (
                environ.get("CUSTOM_FLIPPER_NAME", None)
                or ""
            )
            if (log_custom_fz_name != "") and (len(log_custom_fz_name) <= 8) and (log_custom_fz_name.isalnum()) and (log_custom_fz_name.isascii()):
                self.logger.info(
                    f"Flipper Custom Name is set:\n\tName: {log_custom_fz_name} : length - {len(log_custom_fz_name)} chars"
                )

            if (bundle_result := UpdateMain(no_exit=True)(bundle_args)) == 0:
                self.logger.info(
                    fg.boldgreen(
                        f"Use this directory to self-update your Flipper:\n\t{bundle_dir}"
                    )
                )

                # Create tgz archive
                with tarfile.open(
                    join(
                        self.output_dir_path,
                        f"{self.DIST_FILE_PREFIX}{bundle_dir_name}.tgz",
                    ),
                    "w:gz",
                    compresslevel=9,
                    format=tarfile.USTAR_FORMAT,
                ) as tar:
                    tar.add(bundle_dir, arcname=bundle_dir_name)

            return bundle_result

        return 0


if __name__ == "__main__":
    Main()()

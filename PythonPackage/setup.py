import re
import subprocess
import shutil
import os.path

from setuptools import setup

def version_from_git_tag():
    """Retrieve version from git history."""
    # Using package version according to PEP 440 -- Version Identification and Dependency Specification
    # https://www.python.org/dev/peps/pep-0440/#local-version-identifiers
    pattern = "^v((\\d+)\\.(\\d+)\\.(\\d+)((a|b|rc)\\d+)?(\\.post\\d+)?(\\.dev\\d+)?)(-(\\d+)-g([0-9a-f]{7}))?$"
    try:
        describe = subprocess.check_output(["git", "describe", "--tags", "--match", "v*", "--always"]).rstrip().decode()
    except subprocess.CalledProcessError:
        return "0.0.0+nogit"

    match = re.match(pattern, describe)
    if not match:
        return f"0.0.0+git{describe}"
    if match.group(10) and match.group(11):
        return f"{match.group(1)}+git{match.group(10)}.{match.group(11)}"
    return match.group(1)

# Copy GenericNode.h before calling setup
# so that the right version is included in the templates


setup (name = 'cmsis-stream',
       version = version_from_git_tag(),
       packages=["cmsis_stream",
                 "cmsis_stream.cg",
                 "cmsis_stream.cg.nodes",
                 "cmsis_stream.cg.sds",
                 "cmsis_stream.cg.yaml",
                 "cmsis_stream.cg.nodes.host",
                 "cmsis_stream.cg.scheduler",
                 "cmsis_stream.cg.scheduler.templates",
                 "cmsis_stream.cg.scheduler.templates.reference_code",
                 "cmsis_stream.cg.scheduler.example"],
       description = 'CMSIS-Stream graph description',
       long_description=open("README.md").read(),
       long_description_content_type='text/markdown',
       include_package_data=True,
       author = 'Copyright (C) 2023-2025 ARM Limited or its affiliates. All rights reserved.',
       author_email = 'christophe.favergeon@arm.com',
       url="https://github.com/ARM-software/CMSIS-Stream",
       python_requires='>=3.7',
       license="License :: OSI Approved :: Apache Software License",
       platforms=['any'],
       classifiers=[
              "Programming Language :: Python :: 3",
              "Programming Language :: Python :: Implementation :: CPython",
              "Programming Language :: C",
              "License :: OSI Approved :: Apache Software License",
              "Operating System :: OS Independent",
              "Development Status :: 5 - Production/Stable",
              "Topic :: Software Development :: Embedded Systems",
              "Topic :: Scientific/Engineering :: Mathematics",
              "Environment :: Console",
              "Intended Audience :: Developers",
        ],
        keywords=['development','dsp','cmsis','CMSIS-Stream','Arm','signal processing','maths','ml','cortex-m','cortex-a'],
        install_requires=[
        'networkx>=3.0',
        'jinja2>= 3.1.2, <4.0',
        'sympy>=1.7.1',
        'MarkupSafe>=2.1.2, <3.0',
        'numpy',
        'PyYAML>=6.0'
        ],
        project_urls={  # Optional
           'Bug Reports': 'https://github.com/ARM-software/CMSIS-Stream/issues',
           'Source': 'https://github.com/ARM-software/CMSIS-Stream',
          },
        entry_points={
        'console_scripts': [
            'cmsis-stream = cmsis_stream:cmsis_stream',
        ]
    }
        )
       
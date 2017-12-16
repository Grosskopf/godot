def can_build(platform):
    # should probably change this to only be true on iOS and Android
    return platform == 'uwp'

def configure(env):
    pass

def get_doc_classes():
    return [
        "HololensVRInterface",
    ]

def get_doc_path():
    return "doc_classes"

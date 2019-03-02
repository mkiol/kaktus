import glob, os

dir_name = "images"

# "dir_name": [small, medium, large, app_icon, app_icon_large]
sizes = {
    "z1.0": [32, 64, 96, 86, 108, 540],
    "z1.25": [40, 80, 120, 108, 128, 540],
    "z1.5": [48, 96, 144, 128, 150, 720],
    "z1.5-large": [48, 72, 108, 128, 150, 720],
    "z1.75": [56, 112, 168, 150, 172, 1080],
    "z2.0": [64, 128, 192, 172, 256, 1080],
}

sizes_cover = {
    "z1.0": [234, 374],
    "z1.25": [234, 374],
    "z1.5": [292, 468],
    "z1.5-large": [292, 468],
    "z1.75": [410, 654],
    "z2.0": [410, 654],
}

icons = {
    "vm0": [1],
    "vm1": [1],
    "vm2": [1],
    "vm3": [1],
    "vm4": [1],
    "vm5": [1],
    "vm6": [1],
    "kaktus": [3,4],
    "read" : [1],
    "read-selected" : [1],
    "browser" : [1],
    "webview" : [1],
    "item" : [1],
    "like" : [1],
    "like-selected" : [1],
    "reader" : [1],
    "reader-selected" : [1],
    "night" : [1],
    "night-selected" : [1],
    "rss" : [1],
    "selector" : [1],
    "share" : [1],
    "share-selected" : [1],
    "fontup" : [1],
    "fontdown" : [1],
    "readabove" : [1],
    "friend" : [1],
    "good" : [1],
    "good-selected" : [1],
    "bad" : [1],
    "bad-selected" : [1],
    "filter-0" : [1],
    "filter-2" : [1],
    "filter-1" : [1],
    "pocket" : [1],
    "netvibes" : [1],
    "oldreader" : [1],
    "ttrss" : [1]
}

size_names = {
    0: "s",
    1: "m",
    2: "l",
    3: "a",
    4: "i",
    5: "x",
    6: "c"
}

if not os.path.exists(dir_name):
    os.makedirs(dir_name)

for svg in glob.glob("*.svg"):
    icon_id = svg[:-4]

    if icon_id in icons.keys():
        for d in sizes.keys():
            _dir = "%s/%s" % (dir_name, d)

            if not os.path.exists(_dir):
                os.makedirs(_dir)

            for size_id in icons[icon_id]:
                png = "{}/icon-{}-{}.png".format(_dir, size_names[size_id], icon_id)

                if not os.path.isfile(png):
                    size1 = 0
                    size2 = 0
                    if size_id == 6:
                        size1 = sizes_cover[d][0]
                        size2 = sizes_cover[d][1]
                    else:
                        size1 = sizes[d][size_id]
                        size2 = size1

                    print "File name: %s, size: %dx%d" % (png, size1, size2)
                    print ("inkscape -f {} -e {} -C -w {:.0f} -h {:.0f}"
                              .format(svg, png, size1, size2))
                    os.system("inkscape -f {} -e {} -C -w {:.0f} -h {:.0f}"
                              .format(svg, png, size1, size2))

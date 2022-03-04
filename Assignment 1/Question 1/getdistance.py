from math import radians, cos, sin, asin, sqrt
def haversine(lon1, lat1, lon2, lat2):
    """
    Calculate the great circle distance in kilometers between two points
    on the earth (specified in decimal degrees)
    """
    # convert decimal degrees to radians
    lon1, lat1, lon2, lat2 = map(radians, [lon1, lat1, lon2, lat2])
    # haversine formula
    dlon = lon2 - lon1
    dlat = lat2 - lat1
    a = sin(dlat/2)**2 + cos(lat1) * cos(lat2) * sin(dlon/2)**2
    c = 2 * asin(sqrt(a))
    r = 6371 # Radius of earth in kilometers.
    return c * r

mylat = 12.99
mylon = 80.23
raw_locations = []
with open("locations.txt","r") as f:
    raw_locations = f.readlines()

locations = []
for loc in raw_locations:
    locations.append(tuple(map(float,loc.split(","))))

distances = []
for loc in locations:
    distances.append(haversine(mylon,mylat,loc[1],loc[0]))

with open("distances.txt","w") as f:
    for d in distances:
        f.write(str(d))
        f.write("\n")
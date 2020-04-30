import pcbnew
board = pcbnew.GetBoard()

holes = ["H1","H2","H3","H4","H5","H6"]
coordinates = [{"x":-143.7602170282168,"y":0},{"x":-71.8801085141084,"y":124.5},{"x":-71.8801085141084,"y":-124.5},{"x":0,"y":0},{"x":71.8801085141084,"y":124.5},{"x":71.8801085141084,"y":-124.5},{"x":143.7602170282168,"y":0}]

for i, hole in enumerate(holes):
  print coordinates[i]["x"]
  mod = board.FindModuleByReference(hole)
  mod.SetPosition(pcbnew.wxPointMM( coordinates[i]["x"] ,coordinates[i]["y"]))
  # mod.SetPosition(pcbnew.wxPointMM(10.0*i,150.0*i))

 
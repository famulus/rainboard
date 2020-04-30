const Honeycomb = require('honeycomb-grid')
const makerjs = require('makerjs');
const fs = require('fs');


// this is the bounding box for the buttons
const Hex = Honeycomb.extendHex({
    size: 17.03,           // default: 1
    orientation: 'pointy' // default: 'pointy'
})

// define the honeycomb grid for buttons
const Grid = Honeycomb.defineGrid(Hex)

// the radius here is how many rings of grid are generated
const hexGrid = Grid.hexagon({radius:4})

const points = hexGrid.map(hex =>{  
  return (hex.toPoint()  )
})

// this is the bounding box for the standoffs
const standoffHex = Honeycomb.extendHex({
    size: 83,           // default: 1
    orientation: 'pointy' // default: 'pointy'
})
const standoffGrid = Honeycomb.defineGrid(standoffHex)
const standoffHexGrid = standoffGrid.hexagon({radius:1})
const standoffPoints = standoffHexGrid.map(hex =>{  
  return (hex.toPoint()  )
})



// this is the bounding box for the pcbBoundary

const pcbBoundaryHex = Honeycomb.extendHex({
    size: 89,           // default: 1
    orientation: 'pointy' // default: 'pointy'
})
const pcbBoundaryGrid = Honeycomb.defineGrid(pcbBoundaryHex)
const pcbBoundaryHexGrid = pcbBoundaryGrid.hexagon({radius:1})
const pcbBoundaryPoints = pcbBoundaryHexGrid.map(hex =>{  
  return (hex.toPoint()  )
})


console.log("OK")
// console.log(points.length)
console.log(points)
console.log("OK\n\n")
console.log(pcbBoundaryPoints)
console.log(JSON.stringify(standoffPoints))








function myModel() {

  // console.log(points)
  // const RimCircles = points.map(point => {
  //   return( new makerjs.paths.Circle([point.x, point.y],(27.1/2.0)) )
  // })
  const HoleCircles = points.map(point => {
    return( new makerjs.paths.Circle([point.x, point.y],((24/2.0))) )
  })
  // const ButtonCircles = points.map(point => {
  //   return( new makerjs.paths.Circle([point.x, point.y],(19.5/2.0)) )
  // })

  const standoffCircles = standoffPoints.map(point => {
    return( new makerjs.paths.Circle([point.x, point.y],(3.4/2.0)) )
  })

  // this.paths = RimCircles.concat(ButtonCircles);
  this.paths = HoleCircles.concat(standoffCircles);
  // this.paths = HoleCircles

  // this is the hexagon outline 
  const hexOutline = new makerjs.models.Polygon(6, 160)

debugger
  // hexOutline.layer = "hexOutline"
  this.models = [ hexOutline];


}

const model1 = new myModel();

var svg = makerjs.exporter.toSVG(model1, {useSvgPathOnly:true, units: 'mm'});
// svg = svg.replace('<svg ', '<svg  xmlns="http://www.w3.org/2000/svg" xmlns:xlink= "http://www.w3.org/1999/xlink" ')

// console.log(svg);

const output_file_path = "/Users/clean/Desktop/honeycomb-grid/honeycomb.svg"
fs.writeFile(output_file_path, svg, function(err) {
  if(err) {
    return console.log(err);
  }
});

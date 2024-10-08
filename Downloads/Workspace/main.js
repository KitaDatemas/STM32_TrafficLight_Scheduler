fetch('./treedata.json')
.then(function(resp){
    return resp.json();
})
.then(function(data){
    console.log("Test")
   parentFunction(data);
});

function parentFunction(jsondata){
let mouseX = 0;
let buttonTracker = [];
let rootNode = d3.hierarchy(jsondata, d=>d.children);
let pathLinks = rootNode.links(); 

let updatePathLinks;

let circleLinks = rootNode.descendants();
let updatecircleLinks;

let nameLinks = rootNode.descendants();
let updateNameLinks;

let dim = {
    'width': window.screen.width * (rootNode.height/3.5), 
    'height': window.screen.height * (rootNode.height/20 + 0.0001)*20, 
    'margin':50    
};

let svg = d3.select('#chart').append('svg')
     .style('background', 'white')   
     .attrs(dim);


document.querySelector("#chart").classList.add("center");
let g = svg.append('g')
            .attr('transform', 'translate(140,50)');

    let layout = d3.tree().size([dim.height-50, dim.width-320]);

    layout(rootNode);
   let lines = g.selectAll('path');  



function update(data){

let group =  g.selectAll('path')
    .data(data, (d,i) => d.target.data.name)
    .join(
    function(enter){
        return enter.append('path')
                    .attrs({
                        'd': d3.linkHorizontal()
                        .x(d => d.y)
                        .y(d => d.x),
                        'fill':'none',
                        'stroke':'black'
                    })
    },
    function(update){
        return update;
    },
    function(exit){
        return exit.call(path => path.transition().duration(300).remove()
                                                .attr('d', d3.linkHorizontal()
                                                              .x(d => mouseX)
                                                              .y(d =>d.x)));
    }
)
.call(path => path.transition().duration(1000).attr('d', d3.linkHorizontal()
        .x(d => d.y)
        .y(d => d.x))
        .attr("id", function(d,i){return "path"+i})
        .style("z-index", 0));
}
update(pathLinks); //rootNode.links()

function updatecircles(data){
    g.selectAll("circle")
    .data(data, (d) => d.data.name)
    .join(
        function(enter){
            return enter.append("circle")
                        .style("z-index", 5)
                        .attrs({
                            'cx':(d) => d.y,
                            'cy':(d) => d.x,
                            'r': 12,
                            'fill':(d) => {
                                if(d.children == undefined){
                                    return 'yellow'
                                }
                                return 'green'
                            },
                            'id': (d,i) =>d.data.name,
                            'class':'sel'                      
                        })
        },
        function(update){
            return update;
        },
        function(exit){
            return exit.call(path => path.transition().duration(100).remove()
            .attrs({
                'cx':(d) =>d.y,
                'cy':(d) =>d.x,
                'r' : 12
            }));
        }
    )
    .call(circle => circle.transition().duration(100).attr('cx', (d)=>d.y))

    .on('mouseover', function(d, event){
        d3.select(this)
            .attrs({                
               'fill':'orange',
            })
        .transition().duration(100).attr('circle');
        let table = d3.select("body")
        .append("div")
        .attr("id","displayBlock")
        .attr("class", "dBlock");
        table.append("h3")
        .text("Checkout tab")
        table.append("p")
        .attr("class", "dBlock")
        .text("Work name: " + d.data.name);
        table.append("p")
        .attr("class", "dBlock")
        .text("Publisher: " + d.data.publisher);
    })
    .on('mouseout', function(d){
        d3.select(this)
        .attr('fill', (d)=>{
            if(d.children == undefined){
                return 'yellow';
            }
        return 'green'
        })
        .transition().duration(100).attrs({
            'r' : 12
        })
        d3.select("#displayBlock")
            .transition()
            .duration(200)
            .style("opacity", 0)
            .on("end", function() { 
            d3.select(this).remove();
            });
    })
    .on('click', async function(d){
        let buttonId = d3.select(this)["_groups"][0][0]["attributes"].id.value;
        mouseX = d3.select(this)["_groups"][0][0]["attributes"].cx.value;
        let checkButtonExists = buttonTracker.filter(button => button.buttonId == buttonId);
        if(checkButtonExists[0]!=undefined){
            buttonTracker = buttonTracker.filter(button => button.buttonId != buttonId);
            pathLinks = checkButtonExists[0].buttonPathData.concat(pathLinks);
            update(pathLinks);
            circleLinks = checkButtonExists[0].buttoncircleData.concat(circleLinks);
            updatecircles(circleLinks);
            nameLinks =checkButtonExists[0].buttonTextData.concat(nameLinks);
            updateText(nameLinks);
            return;
        }

        var valueArray = await processedlinks(d.links());   

        updatePathLinks = pathLinks.filter(function(item){        
            return !valueArray.includes(item.target.data.name);                                       
        });

        //now run the filter to get unfiltered items
        var clickedPathData = pathLinks.filter(function(item){
            return valueArray.includes(item.target.data.name);
        });

        updatecircleLinks = circleLinks.filter(function(item){
            return !valueArray.includes(item.data.name);
        });

        var clickedcircleData = circleLinks.filter(function(item){
            return valueArray.includes(item.data.name);
        });
    
        updateNameLinks = nameLinks.filter(function(item){
            return !valueArray.includes(item.data.name);
        });

        var clickedTextData = nameLinks.filter(function(item){
            return valueArray.includes(item.data.name);
        });

        //now we push the circleData to an array
        buttonTracker.push({
            buttonId: buttonId,
            buttonPathData: clickedPathData,
            buttoncircleData: clickedcircleData,
            buttonTextData: clickedTextData
        })
        
        update(updatePathLinks);
        updatecircles(updatecircleLinks);
        updateText(updateNameLinks);
        async function processedlinks(dlinks) {
        var valueArray = [];
            return new Promise((resolve, reject)=>{
                dlinks.forEach(async(element) =>{
                    valueArray.push(element.target.data.name); 
                });
                resolve(valueArray);      
            });
        }
        pathLinks = updatePathLinks;
        circleLinks = updatecircleLinks;
        nameLinks = updateNameLinks;
    });
}

updatecircles(rootNode.descendants());

function updateText(data){
    g.selectAll('text')
        .data(data, (d) =>d.data.name)
        .join(
            function(enter){
                return enter.append('text')
                            .attrs({
                                'x' : (d) => 0,
                                'y' : (d) => d.x,
                                'font-size': 0
                            })
                            .text((d) => d.data.name);
            },
            function(update){
                return update;
            },
            function(exit){
                return exit.call(text => text.transition().duration(100).remove().attrs({
                    'x':(d) => 0,
                    'y':(d) => d.x,
                    'font-size':0 
                }));   
            }
        )
        .call(text => text.transition().duration(300).attrs({
            'x':(d) =>d.y - 15,
            'y':(d) =>d.x - 11,
            'font-size':15,
            'fill':'black',
        }));
}

updateText(nameLinks);

}
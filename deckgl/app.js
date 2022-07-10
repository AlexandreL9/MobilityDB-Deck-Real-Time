import React, {useState, useEffect} from 'react';
import {render} from 'react-dom';
import {StaticMap} from 'react-map-gl';
import DeckGL from '@deck.gl/react';
import {PathLayer} from '@deck.gl/layers';
import {IconLayer} from '@deck.gl/layers';
import FPSStats from "react-fps-stats";


const INITIAL_VIEW_STATE = {
  longitude: -73.860749,
  latitude: 40.684761,
  zoom: 15,
  pitch: 45,
  bearing: 0
};

const _THEME = [
[100, 250, 93],
[23, 165, 137],
[100, 250, 150],
[100, 150, 255],
[250, 50, 250],
[255, 50, 50]
];

const MAP_STYLE = 'https://basemaps.cartocdn.com/gl/voyager-gl-style/style.json';



export default function App({
	initialViewState = INITIAL_VIEW_STATE,
	mapStyle = MAP_STYLE,
	}) {
		
	var last_update_time = {};

	function timeout(ms) {
		return new Promise(resolve => setTimeout(resolve, ms));
	}
	
	function updateDisplayInfo(trajectoryNumber, currentTimestamp){
		var tNumber = document.getElementById('tNumber').innerHTML = trajectoryNumber;
		var timeDiv = document.getElementById('timeDiv').innerHTML = currentTimestamp;
	}
	
	async function* getData() {
		var resp = []; // start with empty segment
		while(1){
			fetch("http://localhost:8003/trips_last.json")
				.then((response) => response.json())
				.then((response) => {
					if (Object.keys(response).length>0){
						updateDisplayInfo(response.length, Math.floor(Date.now() / 1000));
						let lengthRespTMP = response.length;
						let pathID = 0;
						
						while(pathID < lengthRespTMP){
							let positionID = 0;
							if(!(parseInt(response[pathID]["vehicleID"]) in last_update_time)){
								last_update_time[parseInt(response[pathID]["vehicleID"])] = 1;	
							}
							let lengthTMP = response[pathID]["timestamps"].length;

							while(positionID < lengthTMP){
								if (parseInt(response[pathID]["timestamps"][positionID]) < last_update_time[parseInt(response[pathID]["vehicleID"])]){
									response[pathID]["timestamps"].splice(positionID, 1);
									response[pathID]["path"].splice(positionID, 1);
								}
								else{
									if(parseInt(response[pathID]["timestamps"][positionID]) > last_update_time[parseInt(response[pathID]["vehicleID"])	]){
										last_update_time[parseInt(response[pathID]["vehicleID"])] = parseInt(response[pathID]["timestamps"][positionID]);
									}
									positionID++;
								}
								if(response[pathID]["timestamps"].length === 0 || response[pathID]["path"].length === 0){
									lengthTMP = 0;
								}
								else{
									lengthTMP = response[pathID]["timestamps"].length;
								}
							}
							if(response[pathID]["timestamps"].length === 0 || response[pathID]["path"].length === 0){
								response.splice(pathID, 1);
							}
							else{
								pathID++;
							}
							lengthRespTMP = response.length;	
						}
					}
					if(response.length === 0){
						resp = [];
					}
					else{
						resp = response;
					}
				});
			await timeout(1000);
			yield resp;
		}
	}
	
    const dataVar = {data: getData()};

    function updateLayers() {
		const { data } = dataVar;
		const layers =[
			new PathLayer({
				id: 'PathLayer',
				data,
				getPath: d => d.path,
				parameters: {
					depthMask: false
				},
				getWidth: d => 2,
				widthMinPixels: 2,
				widthScale: 2,
				getColor: d => (_THEME[(Math.floor(Date.now() / 1000))%6]),
				opacity: 0.2,
				pickable: true,
				autoHighlight: true,
				shadowEnabled: false
			})
		];
		return layers;
	}

	const _layers = updateLayers();
		
	return (
	<DeckGL
	  layers={_layers}
	  initialViewState={initialViewState}
	  controller={true}
	  getTooltip={({object}) => object && `Vehicle ID : ${object.vehicleID}`}
	>
	  <StaticMap reuseMaps mapStyle={mapStyle} preventStyleDiffing={true} />
	  
	</DeckGL>
	);
}

export function renderToDOM(container) {

  render(<div><App /><FPSStats /></div>, container);
	
}

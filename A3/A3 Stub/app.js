'use strict'

// C library API
const ffi = require('ffi');

// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

app.use(fileUpload());

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/',function(req,res){
  res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
  fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {

  let uploadFile = req.files.uploadFile;
  
  if(!req.files  || uploadFile == undefined) {

    res.redirect('/'); 
    return res.status(400).send('No file was uploaded.'); 
  }

  console.log(uploadFile.name);
  // Use the mv() method to place the file somewhere on your server
  uploadFile.mv('uploads/' + uploadFile.name, function(err) {
    if(err) {
      return res.status(500).send(err);
    }

    res.redirect('/');
  });

});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
  fs.stat('uploads/' + req.params.name, function(err, stat) {
    console.log(err);
    if(err == null) {
      res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
    } else {
      res.send('');
    }
  });
});

//******************** Your code goes here ******************** 

let sharedLib = ffi.Library('sharedLib', {
  'GEDCOMtoJSON': [ 'string', ['string' ] ],
  'getIndividualsFromGEDCOM': [ 'string', ['string' ] ],
  'createNewGEDCOM':['string', ['string','string']]  									
});




app.get('/getFiles', function(req,res){

  let files = getFilesFromServer();
  res.send({
    blah: files
  });



});

app.get('/getFileInfo', function(req,res){

	let fileName = req.query.fileName;
	let GEDCOMobject = sharedLib.GEDCOMtoJSON(fileName);

	res.send({
		fileInfo: GEDCOMobject
	});

});

app.get('/gedView', function(req,res){

	let fileName =req.query.fileName;
  let data = sharedLib.getIndividualsFromGEDCOM(fileName);

  res.send({
    data: data
  });


});

app.get('/createGEDCOM', function(req,res){

  let newGEDCOM = req.query.newGEDCOM;
  let newGEDCOMFileName = req.query.fileName;
  let fileExt = newGEDCOMFileName.split('.').pop();

  let response = "";
  let GEDCOMobject = {};

  if(fileExt == "ged" && isUniqueFile(newGEDCOMFileName)==true){

    response = sharedLib.createNewGEDCOM(newGEDCOM, newGEDCOMFileName);
    GEDCOMobject = sharedLib.GEDCOMtoJSON(newGEDCOMFileName);


  }else{
    response = "writeStatus: filename is invalid or file already exists on the server";
  }

  res.send({
    response: response,
    GEDCOMobject: GEDCOMobject
  });


});

app.get('/addIndividual', function(req,res){

  let givenName = req.query.givenName;
  let surname = req.query.surname;
  let fileName = req.query.fileName;

  let response = sharedLib.addIndividualToGEDCOM(fileName, givenName, surname);


});



function getFilesFromServer(){

  let files = fs.readdirSync('./uploads/');
  let filteredFiles = filterFiles(files);
  return filteredFiles;
}

function isUniqueFile(fileName){

  let files = getFilesFromServer();

  for(var i in files){

    if(fileName == files[i]){
      return false;
    }

  } 
  return true;

}

function filterFiles(files){
  let filteredFiles = [];
  for (var i in files) {

    let fileExt = files[i].split('.').pop();

    if(fileExt == 'ged'){
      filteredFiles.push(files[i]);
    }     

  }
  return filteredFiles;

}
app.listen(portNum);
console.log('Running app at localhost: ' + portNum);
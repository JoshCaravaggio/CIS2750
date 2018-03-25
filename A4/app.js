'use strict'

// C library API
const ffi = require('ffi');
// mySQL js library
const mysql = require('mysql');
var connection;

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

 // console.log(uploadFile.name);
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
  'createNewGEDCOM':['string', ['string','string']],  		
  'addIndividualToGEDCOM':['string', ['string','string']],  
  'getDescendantsFromGEDCOM':['string', ['string','string', 'int']],
  'getAncestorsFromGEDCOM':['string', ['string','string', 'int']]
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

  let fileName = req.query.fileName;

  let indDataObj = {
    givenName: req.query.givenName,
    surname: req.query.surname
  }

  let indData = JSON.stringify(indDataObj);

  let response = sharedLib.addIndividualToGEDCOM(fileName, indData);

  res.send({
    response: response
  });


});

app.get('/getDescendants', function(req,res){

  let fileName = req.query.fileName;

  let indDataObj = {
    givenName: req.query.givenName,
    surname: req.query.surname
  }
  let maxGen = req.query.maxGen;
 // console.log(maxGen);
  let indData = JSON.stringify(indDataObj);
  let response = sharedLib.getDescendantsFromGEDCOM(fileName, indData, maxGen);
  //console.log(response);

  res.send({
    response: response
  });


});

app.get('/getAncestors', function(req,res){

  let fileName = req.query.fileName;

  let indDataObj = {
    givenName: req.query.givenName,
    surname: req.query.surname
  }

  let maxGen = req.query.maxGen;
  let indData = JSON.stringify(indDataObj);
  //console.log(maxGen);
  let response = sharedLib.getAncestorsFromGEDCOM(fileName, indData, maxGen);
  //console.log(response);

  res.send({
    response: response
  });


});

app.get('/setConnection', function(req,res){

  let username =req.query.username;
  let password =req.query.password;
  let dbName =req.query.dbName;
  let host =  'dursley.socs.uoguelph.ca';

  connection = mysql.createConnection({
    host : host,
    user : username,
    password : password,
    database : dbName

  });

  connection.connect();
  connection.query("SELECT * FROM INDIVIDUAL", function(err, rows, fields){
    if(err){
      res.send({
        status: "FAIL"

      });

    }else{
      res.send({
        status: "OK"

      });      
    }

  });

});

app.get('/storeFilesToDB', function(req,res){

  let files = getFilesFromServer();
  var GEDCOMobject;
  var indList;

  for(let file of files){

      //console.log("Checking for file " + file);
      isFileInDB(file, function(data){
        //console.log(data);
        if(data == false){

          //console.log(file + " was not found in the database. Adding");
          GEDCOMobject = JSON.parse(sharedLib.GEDCOMtoJSON(file));
          saveFileToDB(GEDCOMobject);

          getFileKey(file ,function(err, data){

            indList = JSON.parse(sharedLib.getIndividualsFromGEDCOM(file));
            let sourceFileID = data;

            for(let ind of indList){   

              saveIndToDB(ind, sourceFileID);

            }

        });
      }

    });
  }
  res.send({
    data: "data"

  });

});

app.get('/clearFilesFromDB', function(req, res){


    connection.query("DELETE FROM INDIVIDUAL", function(err, rows, fields){
      connection.query("ALTER TABLE INDIVIDUAL AUTO_INCREMENT = 1", function(err, rows, fields){


    
      });

  
    });
    connection.query("DELETE FROM FILE", function(err, rows, fields){

      connection.query("ALTER TABLE FILE AUTO_INCREMENT = 1", function(err, rows, fields){


    
      });

    });
  res.send({
    data: "data"

  });


});

function checkDBIndividuals(){

    connection.query("SELECT * FROM INDIVIDUAL", function(err, rows, fields){
   
      console.log("Individuals");     
      for(let row of rows){
        console.log(row);
      }


    }); 

}

app.get('/manualQuery', function(req,res){

  let query = req.query.queryString;

  let response = getQuery(query, function(err, data){
    if(err){
      res.send({
        err: err,
        data: null
      });
    }else{
      res.send({
        err: null,
        data: data
      });

    }
  });

});

app.get('/stdQuery', function(req,res){

  let query = req.query.queryString;

  let response = getQuery(query, function(err, data){
    if(err){
      res.send({
        err: err,
        data: null
      });
    }else{
      res.send({
        err: null,
        data: data
      });

    }
  });

});

function getQuery(query, callback){

  queryDB(query, function(err, data){
    if(err){
      callback(err, null);
    }else{
      callback(null, data);
    }
  });

}
function checkDBFiles(){
    connection.query("SELECT * FROM FILE", function(err, rows, fields){

    console.log("Files");        
      for(let row of rows){
        console.log(row);
      }            
    });
}

function saveFileToDB(object){

  let fileName = object.fileName;
  let source = object.source;
  let version = object.version;
  let encoding = object.encoding;
  let submitterName = object.submitterName;
  let submitterAddress = object.submitterAddress;
  let numIndividuals = object.numIndividuals;
  let numFamilies = object.numFamilies;

  let query = "INSERT INTO FILE (file_Name, source, version, encoding, sub_name, sub_addr, num_individuals, num_families) VALUES (";
  query = query + "'"  + fileName  + "',";
  query = query + "'" + source  + "',";
  query = query + "'" + version  + "',";  
  query = query + "'"  + encoding  + "',";
  query = query + "'" + submitterName  + "',";
  query = query + "'" + submitterAddress  + "',"; 
  query = query + numIndividuals + ",";
  query = query + numFamilies + ")";

  connection.query(query, function(err, rows, fields){


  });


}
function saveIndToDB(object, sourceFile){

  let givenName = object.givenName;
  let surname = object.surname;
  let sex = object.sex;
  let famSize = 0;

  let query = "INSERT INTO INDIVIDUAL (surname, given_name, sex, fam_size, source_file) VALUES(";
  query = query + "'"  + surname  + "',";
  query = query + "'" + givenName  + "',";
  query = query + "'" + sex  + "',";  
  query = query  + famSize  + ",";
  query = query +  sourceFile  + ")";  

  connection.query(query, function(err, rows, fields){

  });


}
function getFileKey(file, callback){

  let query = "SELECT file_id FROM FILE WHERE file_Name = '" + file + "'";
  var id;

  connection.query(query, function(err, rows, fields){
    if(err){
      callback(err, null);
    }else{
      callback(null, rows[0]['file_id']);
    }     
  });


}
function isFileInDB(file, callback){

  let query = "SELECT COUNT(file_Name) FROM FILE WHERE file_Name = '" + file + "'";

  connection.query(query, function(err, rows, fields){
     
    let response = rows[0]['COUNT(file_Name)'];
    if(response == 0){      
      callback(false);
    }else{
      callback(true);
    }

  });

}
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

function queryDB(query, callback){

  connection.query(query, function(err, rows, fields){

    if(err){
      callback(err, null);
    }else{

      callback(null, rows);
    }

  });


}
app.listen(portNum);
console.log('Running app at localhost: ' + portNum);
$(document).ready(function(){

    addToStatusPanel("GEDCOM parser application by Joshua Caravaggio");
    getDBCredentials();

    $.ajax({
        type: 'get',
        dataType: 'json',
        url: '/getFiles',
        success : function(data){

            var fileCounter = 0;

            for(var i in data){
                for(var j in data[i]){

                    $.ajax({
                        type: 'get',
                        dataType: 'json',
                        url: '/getFileInfo',
                        data: {
                            'fileName': data[i][j]
                        },
                        success: function(newData){

                            fileInfo = JSON.parse(newData.fileInfo);

                            if(fileInfo.fileName !="INVALID_FILE"){

                                addFileToSelect(fileInfo.fileName);
                                fileCounter++;
                                addGEDCOMtoFileLog(fileInfo, fileCounter);
                                addToStatusPanel("Loaded " + fileInfo.fileName);    

                            }

                        },
                        fail: function(newData){

                            console.log("Failure");
   
                        }
                    });
                    

                }
            }

        }, fail: function(data){

            console.log("Request broken");
        }
    });

    $('#createGEDCOM').click(function(){

        let createForm = document.getElementById("createGEDCOMForm");
        let createFileName = createForm.createFileName.value;
        let createSubName = createForm.createSubName.value;
        let createSubAddr = createForm.createSubAddr.value;               
    

        if(createFileName != "" && createSubName != "" && createSubAddr != ""){

            let newGEDCOM = {
            "source":"GEDCOM Web App",
            "gedcVersion":"5.5",
            "encoding":"ASCII",
            "subName":createSubName,
            "subAddress":createSubAddr
            }        

            let newGEDCOMdata = JSON.stringify(newGEDCOM);


            $.ajax({

                type: 'get',
                dataType: 'json',
                url: '/createGEDCOM',
                data: {
                    newGEDCOM: newGEDCOMdata,
                    fileName: createFileName
                },
                success: function(newData){

                    response = newData.response;


                    if(response=="OK"){

                        fileInfo = JSON.parse(newData.GEDCOMobject);
                        let rowCount = document.getElementById("fileLogTable").rows.length;
                        addToStatusPanel("Successfully wrote " + createFileName + " to the server");
                        addFileToSelect(createFileName);
                        addGEDCOMtoFileLog(fileInfo, rowCount);  

                    }else{
                        addToStatusPanel("Failed to write " + createFileName + " to server");

                    }                 
                }

            });
        }else{
            addToStatusPanel("Create GEDCOM error: fields cannot be empty");
        }   


    });    

    $('#addIndividual').click(function(){

        let createForm = document.getElementById("addIndividualForm");
        let givenName = createForm.givenName.value;
        let surname = createForm.surname.value;               
        let fileName = createForm.addIndFileSelect.value;

        if(givenName == "" && surname == ""){
            addToStatusPanel("Cannot add individual: atleast one name field must be present.");

        }else{


        $.ajax({

            type: 'get',
            dataType: 'json',
            url: '/addIndividual',
            data: {
                givenName: givenName,
                surname: surname,
                fileName: fileName
            },
            success: function(newData){

                response = newData.response;
                addToStatusPanel("Successfully added " +givenName + " " + surname + " to " + fileName);

            }

        });

        }


    });

    $('#getDescendants').click(function(){

        let getDesForm = document.getElementById('getDescendantsForm');
        let givenName = getDesForm.givenName.value;
        let surname = getDesForm.surname.value;        
        let maxGen = getDesForm.maxGen.value;   
        let fileName = getDesForm.getDesFileSelect.value;

        if(givenName == "" && surname == ""){
            addToStatusPanel("Cannot get descendants: atleast one name field must be present.");

        }else if(!(Number.isInteger(eval(maxGen))) || (eval(maxGen) <0)){

            if(!(Number.isInteger(maxGen))){
                addToStatusPanel("Cannot get descendants: entered value is non-numeric or negative: " + maxGen);
            }else{

                addToStatusPanel("Cannot get descendants: Number of generations must be greater than or equal to 0.");
            }
        }else{

        $.ajax({

            type: 'get',
            dataType: 'json',
            url: '/getDescendants',
            data: {
                givenName: givenName,
                surname: surname,
                fileName: fileName,
                maxGen: maxGen               
            },
            success: function(newData){

                response = newData.response;

                if(response == "IND_NOT_FOUND"){

                    addToStatusPanel(givenName + " " + surname + " was not found in file " + fileName);

                }else{

                    let descendantsTable = document.getElementById('getDesTable');
                    let rowCount = descendantsTable.rows.length;
                    clearTable(descendantsTable,0);
                    if(maxGen == 0){
                        addToStatusPanel("Successfully retrieved all generations of " + givenName + " " + surname + "'s descendants");

                    }else{
                        addToStatusPanel("Successfully retrieved " + maxGen + " generation(s) of " + givenName + " " + surname + "'s descendants");
                    }
                    let generationList = JSON.parse(response);
        

                    for(var i in generationList){
              
                        addGenerationToTable(descendantsTable,i, generationList[i]);

                    }
                }
            }

        });

        }


    });     

    $('#getAncestors').click(function(){

        let getAnsForm = document.getElementById('getAncestorsForm');
        let givenName = getAnsForm.givenName.value;
        let surname = getAnsForm.surname.value;        
        let maxGen = getAnsForm.maxGen.value;   
        let fileName = getAnsForm.getAnsFileSelect.value;

        if(givenName == "" && surname == ""){
            addToStatusPanel("Cannot get ancestors: atleast one name field must be present.");

        }else if(!(Number.isInteger(eval(maxGen))) || (eval(maxGen) <0)){

            if(!(Number.isInteger(maxGen))){
                addToStatusPanel("Cannot get ancestors: entered value is non-numeric or negative: " + maxGen);
            }else{

                addToStatusPanel("Cannot get ancestors: Number of generations must be greater than or equal to 0.");
            }
        }else{

        $.ajax({

            type: 'get',
            dataType: 'json',
            url: '/getAncestors',
            data: {
                givenName: givenName,
                surname: surname,
                fileName: fileName,
                maxGen: maxGen               
            },
            success: function(newData){

                response = newData.response;

                if(response == "IND_NOT_FOUND"){

                    addToStatusPanel(givenName + " " + surname + " was not found in file " + fileName);

                }else{

                    let ancestorsTable = document.getElementById('getAnsTable');
                    let rowCount = ancestorsTable.rows.length;
                    clearTable(ancestorsTable,0);
                    if(maxGen == 0){
                        addToStatusPanel("Successfully retrieved all generations of " + givenName + " " + surname + "'s ancestorss");

                    }else{
                        addToStatusPanel("Successfully retrieved " + maxGen + " generation(s) of " + givenName + " " + surname + "'s ancestors");
                    }
                    let generationList = JSON.parse(response);

                    for(var i in generationList){
                        addGenerationToTable(ancestorsTable,i, generationList[i]);

                    }
                }
            }

        });

        }


    });

    $('#connectToDB').click(function(){

        let responseElement = document.getElementById("connectionResponse");
        let connectionForm = document.getElementById("dbCredentialForm");
        let username = connectionForm.username.value;
        let password = connectionForm.password.value;
        let dbName = connectionForm.dbName.value;
        let connectionResponse = document.getElementById("connectionResponse");


        $.ajax({
            type: 'get',
            dataType: 'json',
            url: '/setConnection',
            data: {
                username: username,
                password: password,
                dbName: dbName,             
            },
            success : function(data){

                if(data.status == "FAIL"){
                    connectionResponse.innerHTML =  "<p>Connection could not be established.<br>Ensure your credentials are correct</p>";
                }else{

                    let popupWindow = document.getElementById("dbCredPopup");
                    popupWindow.style.display = "none";
                    let query = "create table IF NOT EXISTS FILE(file_id int not null AUTO_INCREMENT, file_Name VARCHAR(60) not null, source VARCHAR(15) not null, version VARCHAR(10) not null, encoding VARCHAR(10) not null, sub_name VARCHAR(62) not null , sub_addr VARCHAR(256),num_individuals INT, num_families INT, PRIMARY KEY(file_id))";
                    $.ajax({

                        type: 'get',
                        dataType: 'json',
                        url: '/manualQuery',
                        data:{
                            queryString: query
                        },
                        success : function(err, data){
                         
                            if(!(err.err)){
                                for(let i in err.data){
                                    addToDBResults(err.data[i]);

                                }
                                query = "create table IF NOT EXISTS INDIVIDUAL ( ind_id int not null AUTO_INCREMENT,  surname VARCHAR(256) not null,  given_name VARCHAR(256) not null, sex VARCHAR(1), fam_size INT ,source_file INT ,primary key(ind_id))";
                                $.ajax({

                                    type: 'get',
                                    dataType: 'json',
                                    url: '/manualQuery',
                                    data:{
                                        queryString: query
                                    },
                                    success : function(err, data){
                                     
                                        if(!(err.err)){
                                            for(let i in err.data){
                                                addToDBResults(err.data[i]);

                                            }

                                        }else{
                                            console.log(err.err);
                                            addToDBResults(err.err.code); 

                                        }                    

                                    }  


                                   }); 
                            }else{
                                console.log(err.err);
                                addToDBResults(err.err.code); 

                            }                    

                        }  


                    });
                       

                }

            }            

        });

    });  

    $('#storeFiles').click(function(){

        var fileInfo;

        $.ajax({
            type: 'get',
            dataType: 'json',
            url: '/storeFilesToDB',
            success : function(data){
                addDBStatusToResults();   

            }            

        });


    });            

    $('#clearFiles').click(function(){

        $.ajax({

            type: 'get',
            dataType: 'json',
            url: '/clearFilesFromDB',
            success : function(data){
                addDBStatusToResults();   

            }  

        });


    });  


    $('#executeManualQuery').click(function(){

        addToDBResults("----------");
    
        let query = document.getElementById('manualQuery').value;
        query = "SELECT " + query;
        $.ajax({

            type: 'get',
            dataType: 'json',
            url: '/manualQuery',
            data:{
                queryString: query
            },
            success : function(err, data){
             
                if(!(err.err)){
                    for(let i in err.data){
                        addToDBResults(err.data[i]);

                    }

                }else{

                    addToDBResults(err.err.code); 

                }                    

            }  


        });

    });    

    $('#displayDBStatus').click(function(){
        addDBStatusToResults();       

    });
    $('#executeQuery1').click(function(){
       
        query = "SELECT * FROM INDIVIDUAL ORDER BY surname";
        addToDBResults("----------");

        $.ajax({

            type: 'get',
            dataType: 'json',
            url: '/stdQuery',
            data:{
                queryString: query
            },
            success : function(err, data){
                
                if(!(err.err)){
                    for(let i in err.data){
                        addToDBResults(err.data[i]);

                    }

                }else{

                    addToDBResults(err.err.code); 

                }                    

            }  


        });

    });   
    $('#executeQuery2').click(function(){
       

        let option = document.getElementById('option2FileSelect');
        let fileName = option.value;
        let query = "SELECT * FROM INDIVIDUAL WHERE source_file = (SELECT file_id FROM FILE WHERE file_Name = '" + fileName  + "')";
        addToDBResults("----------");
        $.ajax({

            type: 'get',
            dataType: 'json',
            url: '/stdQuery',
            data:{
                queryString: query
            },
            success : function(err, data){
    
                if(!(err.err)){
                    for(let i in err.data){
                        addToDBResults(err.data[i]);

                    }

                }else{

                    addToDBResults(err.err.code); 

                }                    

            }  


        });
    });
    $('#executeQuery3').click(function(){
       

        let option = document.getElementById('option3FileSelect');
        let surname = document.getElementById('option3Surname').value;        
        let fileName = option.value;
        let query = "SELECT * FROM INDIVIDUAL WHERE source_file = (SELECT file_id FROM FILE WHERE file_Name = '" + fileName  + "' AND surname = '" + surname + "')";
        addToDBResults("----------");
        $.ajax({

            type: 'get',
            dataType: 'json',
            url: '/stdQuery',
            data:{
                queryString: query
            },
            success : function(err, data){
     
                if(!(err.err)){
                    for(let i in err.data){
                        addToDBResults(err.data[i]);

                    }

                }else{

                    addToDBResults(err.err.code); 

                }                    

            }  


        });

    }); 
    $('#executeQuery4').click(function(){
       

        let option = document.getElementById('option4FileSelect');
        let sex = document.getElementById('option4Sex').value;        
        let fileName = option.value;
        let query = "SELECT * FROM INDIVIDUAL WHERE source_file = (SELECT file_id FROM FILE WHERE file_Name = '" + fileName  + "' AND sex = '" + sex + "')";
        addToDBResults("----------");
        $.ajax({

            type: 'get',
            dataType: 'json',
            url: '/stdQuery',
            data:{
                queryString: query
            },
            success : function(err, data){
     
                if(!(err.err)){
                    for(let i in err.data){
                        addToDBResults(err.data[i]);

                    }

                }else{

                    addToDBResults(err.err.code); 

                }                    

            }  


        });

    }); 

    $('#executeQuery5').click(function(){
       
        let subName = document.getElementById('option5Submitter').value;
        query = "SELECT * FROM FILE WHERE sub_name = '" + subName + "'";
        addToDBResults("----------");

        $.ajax({

            type: 'get',
            dataType: 'json',
            url: '/stdQuery',
            data:{
                queryString: query
            },
            success : function(err, data){
                
                if(!(err.err)){
                    for(let i in err.data){
                        addToDBResults(err.data[i]);

                    }

                }else{

                    addToDBResults(err.err.code); 

                }                    

            }  


        });

    });
    $('#DBhelp').click(function(){
       
        query = "DESCRIBE FILE";
        addToDBResults("----------");
        addStringToDBResults("FILE information:");

        $.ajax({

            type: 'get',
            dataType: 'json',
            url: '/stdQuery',
            data:{
                queryString: query
            },
            success : function(err, data){
                //CON
                if(!(err.err)){
                    for(let i in err.data){
                        addToDBResults(err.data[i]);

                    }
                    query = "DESCRIBE INDIVIDUAL"
                    addStringToDBResults("INDIVIDUAL information:");

                    $.ajax({

                        type: 'get',
                        dataType: 'json',
                        url: '/stdQuery',
                        data:{
                            queryString: query
                        },
                        success : function(err, data){
                            //CON
                            if(!(err.err)){
                                for(let i in err.data){
                                    addToDBResults(err.data[i]);

                                }


                            }else{

                                addToDBResults(err.err.code); 

                            }                    

                        }  


                    });

                }else{

                    addToDBResults(err.err.code); 

                }                    

            }  


        });

    });      

});


$(document).on('change', '#GEDViewSelect',function(e){


    let select = e.target;
    let fileName = select.value;
    let gedTable = document.getElementById('gedViewTable');
    clearTable(gedTable,1);

    $.ajax({

        type: 'get',
        dataType: 'json',
        url: '/gedView',
        data: {
            fileName: fileName
        },

        success: function(newData){

        let indData = JSON.parse(newData.data);

            for(var i in  indData){

                let individual = indData[i];

                addIndividualToGedView(indData[i], eval(i)+eval(1));          

            }
        }


    });
    
});

function addDBStatusToResults(){

    var fileCount;
    var indCount; 
    addToDBResults("----------");
    query = "SELECT COUNT(file_id) FROM FILE";

    $.ajax({

        type: 'get',
        dataType: 'json',
        url: '/stdQuery',
        data:{
            queryString: query
        },
        success : function(err, data){

            if(!(err.err)){

                fileCount = err.data[0]["COUNT(file_id)"];
                query = "SELECT COUNT(ind_id) FROM INDIVIDUAL";
              
                $.ajax({

                    type: 'get',
                    dataType: 'json',
                    url: '/stdQuery',
                    data:{
                        queryString: query
                    },
                    success : function(err, data){

                        if(!(err.err)){

                            indCount= err.data[0]['COUNT(ind_id)'];
                            addStringToDBResults("Database has " + fileCount + " files and " + indCount + " individuals");    

                        }                  

                    }  


                }); 

            }else{


            }                  

        }  


    });

}

function addStringToDBResults(data){

    document.getElementById("resultsArea").innerHTML += data;
    document.getElementById("resultsArea").innerHTML += "<br>";


}
function addToDBResults(data){

    var toAdd = "";

    for(var key in data){
        if(data.hasOwnProperty(key)){
            var value = data[key];
            toAdd += value;
            toAdd += " ";

        }

    }
    let dbScroll = document.getElementById('dbScrollPanel');

    dbScroll.scrollTop = dbScroll.scrollHeight;   
    document.getElementById("resultsArea").innerHTML += toAdd;
    document.getElementById("resultsArea").innerHTML += "<br>";
}
function addIndividualToGedView(individualData, tableRow){

    let gedViewTable = document.getElementById("gedViewTable");
    var newRow   = gedViewTable.insertRow(tableRow);

    var givenNameCell = newRow.insertCell(0);
    givenNameCell.innerHTML = individualData.givenName 

    var surnameCell = newRow.insertCell(1);
    surnameCell.innerHTML = individualData.surname;

    var sexCell = newRow.insertCell(2);
    sexCell.innerHTML = individualData.sex;

}

function addGEDCOMtoFileLog(fileInfo, tableRow){

    let fileLogTable = document.getElementById("fileLogTable");
    var newRow   = fileLogTable.insertRow(tableRow);

    var fileNameCell = newRow.insertCell(0);
    fileNameCell.innerHTML = "<a href = \"/uploads/" + fileInfo.fileName +"\">" + fileInfo.fileName + "</a>";

    var sourceCell = newRow.insertCell(1);
    sourceCell.innerHTML = fileInfo.source;

    var versionCell = newRow.insertCell(2);
    versionCell.innerHTML = fileInfo.version;

    var encodingCell = newRow.insertCell(3);
    encodingCell.innerHTML = fileInfo.encoding;

    var submitterNameCell = newRow.insertCell(4);
    submitterNameCell.innerHTML = fileInfo.submitterName;            

    var submitterAddrCell = newRow.insertCell(5);
    submitterAddrCell.innerHTML = fileInfo.submitterAddress;    

    var numIndividualsCell = newRow.insertCell(6);
    numIndividualsCell.innerHTML = fileInfo.numIndividuals;    

    var numFamiliesCell = newRow.insertCell(7);
    numFamiliesCell.innerHTML = fileInfo.numFamilies;    
    
}
function addFileToSelect(file){

    let getAnsFileSelect = document.getElementById("getAnsFileSelect");
    let getAnsOption = document.createElement("option");
    getAnsOption.text = file;
    getAnsFileSelect.add(getAnsOption);

    let getDesFileSelect = document.getElementById("getDesFileSelect");
    let getDesOption = document.createElement("option");
    getDesOption.text = file;
    getDesFileSelect.add(getDesOption);

    let addIndFileSelect = document.getElementById("addIndFileSelect");
    let addIndOption = document.createElement("option");
    addIndOption.text = file;
    addIndFileSelect.add(addIndOption);

    let gedViewFileSelect = document.getElementById("gedViewFileSelect");
    let gedViewOption = document.createElement("option");
    gedViewOption.text = file;
    gedViewFileSelect.add(gedViewOption);

    let option2FileSelect = document.getElementById("option2FileSelect");
    let option = document.createElement("option");
    option.text = file;
    option2FileSelect.add(option);

    let option3FileSelect = document.getElementById("option3FileSelect");
    option = document.createElement("option");
    option.text = file;
    option3FileSelect.add(option);    

    let option4FileSelect = document.getElementById("option4FileSelect");
    option = document.createElement("option");
    option.text = file;
    option4FileSelect.add(option);
}

function addToStatusPanel(statusString){

    let statusTable = document.getElementById("statusTable");
    let statusScroll= document.getElementById("statusScrollPanel");
    let rowCount = statusTable.rows.length;
    let newRow   = statusTable.insertRow(rowCount);
    let infoCell = newRow.insertCell(0);
    infoCell.innerHTML = "> " + statusString + "<br>"
    statusScroll.scrollTop = statusScroll.scrollHeight;    
}

function clearTable(table, endpoint){

    let rowCount = table.rows.length;
	
	
    for(i = rowCount-1; i>=endpoint; i--){
        table.deleteRow(i);
    }

}

function addGenerationToTable(descendantsTable, row, generationList){

    genNum = eval(row) + eval(1); 
    let newRow   = descendantsTable.insertRow(row);
    let genCell = newRow.insertCell(0);
    genCell.innerHTML = "<b>Generation " + genNum +"</b>:";

    for(var i in generationList){
        genCell.innerHTML = genCell.innerHTML +" " + generationList[i].givenName + " " + generationList[i].surname;

        if((eval(generationList.length) - eval(i))>1){
            genCell.innerHTML = genCell.innerHTML +",";

        }
    }     

}

function getDBCredentials(){

    let popupWindow = document.getElementById("dbCredPopup");
    popupWindow.style.display = "block";


}
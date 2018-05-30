$(document).ready(function(){

    addToStatusPanel("GEDCOM parser application by Joshua Caravaggio");

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
                    clearTable(descendantsTable);
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
                    clearTable(ancestorsTable);
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

});


$(document).on('change', '#GEDViewSelect',function(e){

    let select = e.target;
    let fileName = select.value;
    let gedTable = document.getElementById('gedViewTable');
    clearTable(gedTable);

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

function clearTable(table){

    let rowCount = table.rows.length;

    for(i = rowCount-1; i>=0; i--){
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
#Script to prepare Web files
# - Integrate customization of the application
# - GZip of resultant web files
# - and finally convert compressed web files to C++ header in PROGMEM

#List here web files specific to this project/application
$specificFiles="",""

#list here files that need to be merged with Common templates
$applicationName="WirelessRGBW"
$shortApplicationName="WRGBW"
$templatesWithCustoFiles=@{
    #---Status.html---
    "status.html"=@{
        ApplicationName=$applicationName
        ;
        ShortApplicationName=$shortApplicationName
        ;
        HTMLContent=@'
        <h2 class="content-subhead">RGBW<span id="l3"><h6 style="display:inline"><b> Loading...</b></h6></span></h2>
        Current color : <span id="color"></span><br>
'@
        ;
        HTMLScriptInReady=@'
        $.getJSON("/gs1", function(GS1){

            $.each(GS1,function(k,v){
                $('#'+k).html(v);
            })
            $("#l3").fadeOut();
        })
        .fail(function(){
            $("#l3").html('<h4 style="display:inline;color:red;"><b> Failed</b></h4>');
        });
'@
    }
    ;
    #---config.html---
    "config.html"=@{
        ApplicationName=$applicationName
        ;
        ShortApplicationName=$shortApplicationName
        ;
        HTMLContent=@'
        <h2 class="content-subhead">RGBW<span id="l1"><h6 style="display:inline"><b> Loading...</b></h6></span></h2>
        <form class="pure-form pure-form-aligned" id='f1'>
            <fieldset>

        <input type='hidden' id='m' name='m' value='0'>
        <div class="pure-control-group">
        <label for="ml">Model</label>
        <select id='ml' name='ml' onchange="document.getElementById('m').value=document.getElementById('ml').value;">
            <option value="0">AL-LC01</option>
            <option value="1">AL-LC02</option>
        </select>
    </div>
                <div class="pure-controls">
                    <input type='submit' value='Save' class="pure-button pure-button-primary" disabled>
                </div>
            </fieldset>
        </form>
        <span id='r1'></span>
'@
        ;
        HTMLScript=@'

        $("#f1").submit(function(event){
            $("#r1").html("Saving Configuration...");
            $.post("/sc1",$("#f1").serialize(),function(){ 
                $("#f1").hide();
                var reload5sec=document.createElement('script');
                reload5sec.text='var count=4;var cdi=setInterval(function(){$("#cd").text(count);if(!count){clearInterval(cdi);location.reload();}count--;},1000);';
                $('#r1').html('<h3><b>Configuration saved <span style="color: green;">successfully</span>. System is restarting now.</b></h3>This page will be reloaded in <span id="cd">5</span>sec.').append(reload5sec);
            }).fail(function(){
                $('#r1').html('<h3><b>Configuration <span style="color: red;">error</span>.</b></h3>');
            });
            event.preventDefault();
        });
'@
        ;
        HTMLScriptInReady=@'
        $.getJSON("/gc1", function(GC1){

            $.each(GC1,function(k,v){

                if($('#'+k).prop('type')!='checkbox') $('#'+k).val(v);
                else $('#'+k).prop("checked",v);

                $('#'+k).trigger("change");
            })

            $("#ml").val(GC1.m);

            $("input[type=submit]",$("#f1")).prop("disabled",false);
            $("#l1").fadeOut();
        })
        .fail(function(){
            $("#l1").html('<h6 style="display:inline;color:red;"><b> Failed</b></h6>');
        });
'@
    }
    ;
    #---fw.html---
    "fw.html"=@{
        ApplicationName=$applicationName
        ;
        ShortApplicationName=$shortApplicationName
    }
    ;
    #---discover.html---
    "discover.html"=@{
        ApplicationName=$applicationName
        ;
        ShortApplicationName=$shortApplicationName
    }
}

#call script that prepare Common Web Files and contain compression/Convert/Merge functions
. ..\src\data\_prepareCommonWebFiles.ps1

$path=(Split-Path -Path $MyInvocation.MyCommand.Path)
$templatePath=($path+"\..\src\data")

Write-Host "--- Prepare Application Web Files ---"
Convert-TemplatesWithCustoToCppHeader -templatePath $templatePath -filesAndCusto $templatesWithCustoFiles -destinationPath $path
Convert-FilesToCppHeader -Path $path -FileNames $specificFiles
Write-Host ""
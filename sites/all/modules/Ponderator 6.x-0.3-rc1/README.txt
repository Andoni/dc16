------------------------------------------------------
 PONDERATOR - Idea rating module - v0.3.001 2011/02/08
------------------------------------------------------

Installing

    Copy the module folder into sites/all/modules directory
    Activate it in Administer -> Site building -> Modules. It will automaticaly create neeeded tables in the database.

Configuration by the page admin

    Go to Administer -> Site Configuration -> Ponderator
    You have to define what node type has the ideas and what vocabulary name has the processes names.
    Don't forget to Save the changes.

Configuration by the process manager

    Go to Ponderator Process Configuration.
    Select the process you want to configure.
    Select the Ponderation mode (The way that the users will add the ratings).
        -Depending on the Ponderation mode the possible values will be static or editable.
        -If the values are editable they must be numeric values separated by ',' (for example 1,3,5,9).
    Fill the name, description and weight of the criteria.
        -To add more criteria click on the Add criterion button as many times as you need.
        -If you want to delete a criterion just leave it's name empty.
    Click on the Save button and all changes will be saved.

Userside

    The ponderation functionality will be available here.
    The user must select the process to rate.
    Users will fill the rating matrix to rate the idea-criterion relations.
    Clicking on the Save will save the matrix (if there isn't any error).
    The other tabs will display the ratings and averages in different ways.
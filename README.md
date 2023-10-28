# WAMP: Writing, Annotation, and Marking Platform

WAMP is a web-based annotation tool that tackles the issue of generating annotated corpora by allowing annotators to annotate essays with ease and export the resulting annotated essays for use in GEC research.

## Demo Video
https://github.com/nusnlp/WAMP/assets/67945439/a24cbee0-f12e-4596-8dcb-eb913a4d5b68

## Dependency Versions
- Apache (2.2)
- PHP (5.2)
- MySQL (5.1)
- Ubuntu (9.10)

## Installation
### 1) Docker
1) Install [Docker](https://www.docker.com) on your host environment.
2) Run `docker run --name wamp_docker -it -v $HOST_PATH:$DOCKER_PATH -p $HOST_PORT_NUMBER:$DOCKER_PORT_NUMBER icomputer7/ancient-ubuntu-docker:karmic /bin/bash` to run the [Docker base image](https://github.com/iComputer7/ancient-ubuntu-docker) of Ubuntu 9.10 "Karmic Koala" and to mount the local directory to the Docker container.
    * **$HOST_PATH** is the path to the downloaded "wamp" folder in the host environment.
    * **$DOCKER_PATH** is the desired path for the "wamp" folder in the Docker container.
    * **$HOST_PORT_NUMBER** is the port number of your host environment that is used to communicate with the Docker container.
    * **$DOCKER_PORT_NUMBER** is the port number of your Docker container that is used to communicate with the host environment.
3) Inside the Docker container, run `apt-get update && apt-get install php5 libapache2-mod-php5 php5-cli` to install PHP 5.2.
4) Inside the Docker container, run `apt-get install mysql-server && apt-get install php5-mysql` to install MySQL 5.1.
    * When prompted, set the **$PASSWORD** for the "root" user.
5) Inside the Docker container, install Miniconda2 under `/conda` directory.
6) After activating the `base` Miniconda2 environment, run `pip install nltk==3.4.5` to install NLTK and run `import nltk; nltk.download('punkt')` in Python shell to download the necessary NLTK data under `/conda` directory.

### 2) Apache2
1) Inside the Docker container, configure the Apache2 web server.
2) Once the configuration is complete, run `a2enmod rewrite` to enable the **rewrite** module in Apache2.
3) Run `a2ensite default` to enable the site.
	* If using HTTPS configuration, run `a2enmod ssl` to enable the **ssl** module and run `a2ensite default-ssl` to enable the site.
5) Check that **memory_limit** inside `/etc/php5/apache2/php.ini` is at least **32M** to have sufficient memory limit for PHP.
6) Run `/etc/init.d/apache2 restart` to restart the Apache2 server and apply the new changes.

### 3) MySQL
1) Run `service mysql start` to start the MySQL server.
2) Run `mysqladmin -u root -p create wamp` to create a new database.
    * When prompted, use the **$PASSWORD** set in step 4 of [Docker installation](#1-docker).
3) Run `mysql -u root -p` to connect to the MySQL server.
    * When prompted, use the **$PASSWORD** set in step 4 of [Docker installation](#1-docker).
4) Run the following MySQL command to give necessary privileges to the database.
```
GRANT SELECT, INSERT, UPDATE, DELETE, CREATE, DROP, INDEX, ALTER
ON wamp.*
TO 'root'@'localhost' IDENTIFIED BY '$PASSWORD';
```

### 4) Drupal
1) Inside the Docker container, configure the **$db_url** from "$DOCKER_PATH/wamp/drupal/sites/default/settings.php" accordingly.
    * **$DOCKER_PATH** is the path set in step 2 of [Docker installation](#1-docker).
    * **$PASSWORD** is the password set in step 4 of [Docker installation](#1-docker).
2) Run the following command to make the specified directories world readable, writable and executable.
```
chmod -R a=rwx $DOCKER_PATH/wamp/drupal/files \
&& chmod a=rwx $DOCKER_PATH/wamp/drupal/tmp \
&& chmod a=rwx $DOCKER_PATH/wamp/drupal/sites/default/files
```
3) Using a web browser of your choice, naviagate to the "install.php" path of your site URL to install Drupal (e.g. `http://localhost:8088/wamp/install.php`).
4) Enable Clean URLs
    - Navigate to Administer > Site configuration > Clean URLs
    - Click on "Enabled" and save configuration. 
        - If prompted, click on "run the clean url test"
5) Configure Drupal modules
	- Navigate to Administer > Site building > Modules
	- Enable the required Drupal modules
		- Core - optional
            - Profile
		- Other
            - Conditional Stylesheets
		    - Hovertip
		- User interface
            - jQuery Update
		- WAMP
            - WAMP
            - WAMP Administration
            - WAMP Essay
6) Configure upload directory
	- Navigate to Administer > Site configuration > File system
	- Change File system path to 'files'
7) Configure Profile module
	- Navigate to Administer > User Management > Profiles
	- Add a single-line textfield
		- Choose an appropriate category, e.g., 'Personal Information'
		- Enter 'Name' as the Title.
		- Enter 'profile_pers_name' as the Form name
    - It is of paramount importance that the form name is entered, as is, without quotes.
8) Configure user roles
	- Navigate to Administer > User management > Roles
	- Add a role: 'annotator'
9) Configure user permissions
	- Navigate to Administer > User management > Permissions
	- Assign 'annotate any essay' permission in 'wamp_essay' module to the role 'annotator'
	- Assign 'access wamp' permission in 'wamp' module to the role 'annotator'
10) Configure WAMP
	- Navigate to Administer > Site Configuration > WAMP
	- Choose your operating mode as "Production"
		- Testing mode activates firebug-lite and uses uncompressed javascript for annotator. 
		- Production mode disables firebug-lite and uses minified javascript.

## Usage
### Uploading essays
In order to upload essays into the system, essays need to be in XML format (e.g. "sample.xml").

1) Login as an administrator, navigate to Administer > WAMP > Administration, go to "Import" tab.
2) Upload the XML file and click "Upload"
    * For testing purpose, use the "sample.xml" inside the "wamp" folder.
3) Go to "Administration" tab and under "Operations", click "Process"
4) Under "Process Imported XML", untick "Test Run Only" and click "Process"
5) The page returns a white page with "SimpleXMLElement Object ()", which is expected.
6) Go back to the previous page, and if there is no warning, the essays have been successfully uploaded. Otherwise, the essays may not be uploaded.

### Creating an annotator account
1) Login as an administrator, navigate to Administer > User management > Users, go to "Add user" tab.
2) Enter the details, assign "annotator" role, and click "Create new account"
3) Go to Administer > User management > Users, click "edit" on the newly created annotator account.
4) Go to "Personal Information" tab, and enter the annotator's name.

### Annotating essays
1) Login as an annotator, go to "View Essays" and click on "Annotate" to annotate the essay.
2) Annotate the essay by highlighting the grammatical errors, assigning the appropriate error type, and proposing suitable corrections.

## License
The code in this repository are licensed under the GNU General Public License Version 3 (see [License](./LICENSE.txt)). For commercial use of the source code of WAMP, separate commercial licensing is also available. Please contact Hwee Tou Ng (nght@comp.nus.edu.sg)

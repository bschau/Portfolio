# Sydhavshytten

##Indstillinger

Tilføj kategorier:

Seværdigheder -> (slug) sightseeing
Spisesteder -> (slug) eateries

Tilføj siden:

"Praktisk Info"

Perma-link indstillinger:

"Post-name" (http://localhost/sample-post/) 

Aktiver mod-rewrite i apache:

cd /etc/apache2/mods-enabled/
ln -s ../rewrite.load .
systemctl restart apache2

Tilføj til VirtualHost konfigurationen for sydhavshytten.dk:

	<Directory "/var/www/html">
		RewriteEngine On
		RewriteBase /
		RewriteRule ^index\.php$ - [L]
		RewriteCond %{REQUEST_FILENAME} !-f
		RewriteCond %{REQUEST_FILENAME} !-d
		RewriteRule . /index.php [L]
	</Directory>



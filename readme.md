//// HOW TO USE THIS RAYTRACER ////

use make;

raytracer.exe filename.bmp scene.xml (optional)object.stl


Afin de compiler ce projet vous pouvez utiliser la commande “make” dans le terminal puis la commande  “ ./raytracer.exe filename.bmp filename.xml (optional) filename.stl  “ 

filename.bmp sera le fichier où l’on enregistrera l’image
filename.xml le fichier xml décrivant votre scène 
filename.stl est un fichier stl dont le mesh sera représente dans la scène.

Fichier xml pouvant être compilé

lightScene.xml
pscene1.xml
demo3.xml

/////FICHIER XML////////

Ces fichiers XML comportent les différents éléments qui seront présents sur la scène.
Il comporte une balise <camera> donnant les différents paramètres pour initialiser la caméra
Une balise <objets> pour définir les caractéristiques des différents objets géométriques : sphère, plan, cube, triangle.
Une balise <light> pour définir les caractéristiques des différentes lumières : 
Lumière ponctuelle (les rayons viennent d’un point)
Lumière directionnelle (les rayons viennent de l’infini et sont donc parallèles)

Ce fichier xml est lu dans le main et les différents objets sont stockés dans un vecteur (objet ou light) de la classe “scène”.

Cette scène est ensuite envoyé à la caméra qui va faire le calcul de rayon

//////////// LANCER DE RAYONS ///////////////

Pour chaque pixel, un rayon va être envoyé,
Si ce rayon n’atteint aucun objet, on attribue une couleur par défaut : noir
Si un ou plusieurs objets sont touchés, on prend celui le plus proche. 
	
Afin de savoir quelle couleur va être attribué à ce pixel, on regarde comment l’éclairent les sources lumineuses : 
	Si un ou plusieurs objets se trouvent sur la trajectoire du rayon lumineux, on n’éclaire pas l’objet.
	Sinon on va utiliser la couleur de la source, l’angle d’incidence, la couleur de la sphère, afin de déterminer la couleur du pixel. 


Ce fonctionnement de base  a été amélioré en prenant en compte la réflexivité des objets, ainsi la boucle “while ((coef > 0.0f) && (level < 2)&&objectID!=-1);” permet de faire partir un rayon réfléchi à partir du point d’impact du rayon incident qui subira le même processus que le rayon incident : détection objet, puis détermination de la couleur.

Une fois tous les pixels balayés, la libraire CImg est utilisé pour exporter l’image au format png.

# Semantic Kitti API 

## Classes

### Camera

Classe contenant les méthodes et attributs pour gérer la camera sur la fenêtre de visualisation.

#### Attributs 
- x,y,z : position de la camera
- pitch : angle de rotation haut-bas
- yaw : angle de rotation gauche-droit
- startdrag : booléen qui change quand l'user commence à bouger la souris

#### Méthodes
- lookat : coordonées (x,y,z) vers où la camera pointe
- mousePressed : sauvegarde le x et y de départ où l'utilisateur a cliqué sur la fenêtre, ainsi que l'heure où il l'a fait. Présence d'un booléen pour indiquer le début du drag. 
- mouseReleased : met le drag booléen à false pour indiquer que l'utilisateur a fini de drag.
- Translation / Rotation 
- mouseMoved : mouvement de la camera dans la scene utilisant rotate et translate.


### Laser Scan

Ouvre un fichier binaire pour récup (x,y,z) et la reflectance. Projette ensuite les points si spécifié.

Remission/reflectance/amplitude/intensity : quantité de lumière réfléchis par la surface atteint par le laser.  Témoigne de la distance pour un point donné. 

#### Atributs 

- proj_range : range image projeté de taille (H,W)
- unproj_range : liste des profondeurs de chaque point (H,W)
- proj_xyz : nuage de points (H,W,3)
- proj_remission : reflectance (H,W)
- proj_idx : pour chaque pixel du range image, à quoi je correspond dans le point cloud (H,W)
- proj_x , proj_y : pour chaque point où il se trouve dans le range image

#### Méthodes 
- open_scan : Ouvre un fichier contenant les données numpy (x,y,z) et la reflectance. Stocke ces données dans les tableaux correspondants
- do_range_projection :  Fait une projection sphérique du nuage de points.
    - On commence par récupérer les fov en radian 
    - On prend la profondeur de chaque point
    - On change x et y en projeté
    - On assigne tous les tableaux cités precedemments a des images

### SemLaserscan 
x,y,z,r, sem_label, sem_color, inst_label, inst_color

### LaserScanVis

Classe utilisant vispy pour créer une fenêtre afin de visualiser le nuage de point.

#### Méthodes

- get_mpl_colormap : intialise et retourne la range de couleur 
- update scan : initialise les données pour point cloud 

[Panoptic evaluation](https://kharshit.github.io/blog/2019/10/18/introduction-to-panoptic-segmentation-tutorial) via numpy. 

instance = nombre d'objets presents sur la scène
#### Différents types de segmentation 

- Semantic segmantation = différentie tous les éléments de l'image sans les labeliser 
- Instance segmantation  = met des labels uniquement sur les objets (moto, personnes, poubelle etc)
- Panoptic segmentation = met des labels sur tout ce qu'il y a dans l'image (personne, moto, arbre, ciel etc)

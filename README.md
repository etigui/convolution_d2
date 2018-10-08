# Convolution 2D

Nous allons nous intéresser au traitement d’image de type convolution 2D, ce qui nous permettra de modifier l’aspect visuel d’une image en fonction de paramètres (filtres) prédéfinis. Dans un premier temps, nous allons étudier le principe de fonctionnement de la convolution 2D et mettre en place des fonctions qui permettent d’alléger le traitement de ce type d’opération en utilisant le modèle boss/worker. Ensuite, nous allons comparer chaque résultat de convolution (temps d’exécution) avec une version séquentielle et multi threads afin de constater s’il y a une amélioration du temps de traitement.

## Principe de fonctionnement

Pour effectuer la convolution en appliquant le kernel sur notre image, on s’est inspiré de la fonction suivante :

![Formule de convolution](/images/form.png)

Voici de manière plus concrète comment la fonction précitée a été utilisée pour traiter l’image.

![Convolution](/images/conv.png)

## Calcul des performances
Les tests de performance ont été effectué avec l’image fournie par un ordinateur doté d’un processeur quad core (i72670QM @2.2GHz). Chaque filtre a été appliqué trois fois sur l’image afin d’obtenir les résultats cohérents suivants en milliseconde :

![Performances table](/images/tab.png)

![Performances graph](/images/graph.png)

## Rapport
[Pour plus d‘informations](/doc/Rapport_TP_GuignardE.pdf)

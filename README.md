# 🖼️ PROJET : ENCODEUR / DÉCODEUR D'IMAGE (FORMAT DIF)

## 👤 Auteur
- Nom : Rakotoniary
- Prénom : Loïc
- BUT Informatique, 3e année, Université Gustave Eiffel

*Réalisé individuellement dans le cadre du module de Programmation Avancée.*

---

## Description
Petit projet en C fournissant un encodeur (PGM -> DIF) et un décodeur (DIF -> affichage PGM).
Le moteur de compression utilise :
- codage différentiel,
- repliement (fold/unfold),
- bitstream compacté (pushbits/pullbits).

Le rendu et l'interface sont basés sur la bibliothèque graphique libg2x.

---

## 🚀 Fonctionnalités

### Format DIF (Differential Image Format)
- En-tête binaire structuré (Magic Number `0xD1FF`).
- Gestion des métadonnées (dimensions, quantificateur).

### Algorithme de Compression
- **Transformation en Image Différentielle** : différence pixel N / N-1.
- **Repliement (Folding)** : transformation des différences signées en valeurs non signées.
- **Quantification VLC** : codage à longueur variable basé sur 4 intervalles de probabilité.

### Interface Graphique (GUI)
- Visualisation de l'image avant compression.
- Visualisation de l'image reconstruite après décompression.
- Gestion d'événements clavier pour sauvegarder/quitter.

---

## 🛠️ Installation & Compilation

### Pré-requis
- Compilateur `gcc`
- Bibliothèque `libg2x` (doit être installée ou présente dans `../g2x/`)

### Compilation

Utilisez le Makefile fourni pour générer les deux exécutables :

```sh
make
```

Cela génère :
- `pgmtodif` : L'encodeur
- `diftopgm` : Le décodeur

Pour nettoyer :
```sh
make clean
```

---

## 💻 Utilisation

### 1. Compression (Encodeur)

Convertit une image `.pgm` en fichier `.dif`.

```sh
./pgmtodif <image_source.pgm>
```

**Exemple :**
```sh
./pgmtodif ../../repo/imageExemple.512.pgm
```

- Une fenêtre s'ouvre affichant l'image.
- Appuyez sur `S` pour compresser et sauvegarder.
- Appuyez sur `Q` pour quitter.

### 2. Décompression (Décodeur)

Lit un fichier `.dif` et affiche l'image reconstruite.

```sh
./diftopgm <image_compressee.dif>
```

**Exemple :**
```sh
./diftopgm ../../repo/imageEnxemple.512.dif
```

- Le programme décode le flux binaire.
- Une fenêtre s'ouvre avec l'image reconstruite.
- Appuyez sur `Q` pour quitter.

---

## 🧠 Détails Techniques

### Le Format DIF

Le fichier binaire est structuré comme suit :

| Section       | Taille    | Description                                          |
|---------------|-----------|------------------------------------------------------|
| Header        | 11 octets | Magic Number, Largeur, Hauteur, Infos Quantificateur |
| Premier Pixel | 1 octet   | Valeur brute du premier pixel                        |
| Flux Binaire  | variable  | Suite de codes VLC (Préfixe + Valeur)                |

### Le Quantificateur

Le projet utilise un quantificateur statique optimisé pour les images naturelles (basé sur la distribution statistique des différences) :

| Intervalle | Bits Valeur | Préfixe Huffman | Coût Total |
|------------|-------------|-----------------|------------|
| [0, 2[     | 1           | `0`             | 2 bits     |
| [2, 6[     | 2           | `10`            | 4 bits     |
| [6, 22[    | 4           | `110`           | 7 bits     |
| [22, 256[  | 8           | `111`           | 11 bits    |

### Architecture du Code

```
G2x-CoDec-Project/
├── include/
│   └── codec.h
├── src/
│   ├── codec.c
│   ├── pgmtodif.c
│   └── diftopgm.c
└── Makefile
```
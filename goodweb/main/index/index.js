document.getElementById("open-dtrala").addEventListener("click", function () {
  // Crée un nouvel élément img
  var img = document.createElement("img");
  img.src = "./images/rickroll.gif"; // Chemin vers ton image GIF
  img.alt = "Rickroll";

  // Remplace le contenu de la section "Mandatory" par l'image
  var mandatorySection = document.getElementById("mandatory-section");
  mandatorySection.innerHTML = ""; // Vide le contenu actuel
  mandatorySection.appendChild(img); // Ajoute l'image
});

document.getElementById("open-eviala").addEventListener("click", function () {
  // Crée un nouvel élément img
  var img = document.createElement("img");
  img.src = "./images/non-mario.gif"; // Chemin vers ton image GIF
  img.alt = "Mario";

  // Remplace le contenu de la section "Mandatory" par l'image
  var mandatorySection = document.getElementById("mandatory-section");
  mandatorySection.innerHTML = ""; // Vide le contenu actuel
  mandatorySection.appendChild(img); // Ajoute l'image
});

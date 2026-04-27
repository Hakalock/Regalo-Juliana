const fotos = [
  "imagenes/foto1.png",
  "imagenes/foto2.png",
  "imagenes/foto3.png",
  "imagenes/foto4.png",
  "imagenes/foto5.png"
];

const imagen = document.getElementById("fotoCarrusel");
const botonAnterior = document.getElementById("anterior");
const botonSiguiente = document.getElementById("siguiente");
const puntos = document.getElementById("puntos");

let indiceActual = 0;
let temporizador;

fotos.forEach((_, indice) => {
  const punto = document.createElement("button");
  punto.className = "punto" + (indice === 0 ? " activo" : "");
  punto.setAttribute("aria-label", "Ver foto " + (indice + 1));
  punto.addEventListener("click", () => mostrarFoto(indice));
  puntos.appendChild(punto);
});

function mostrarFoto(indice) {
  indiceActual = (indice + fotos.length) % fotos.length;
  imagen.style.animation = "none";
  void imagen.offsetWidth;
  imagen.src = fotos[indiceActual];
  imagen.style.animation = "aparecer 0.7s ease";

  [...puntos.children].forEach((punto, i) => {
    punto.classList.toggle("activo", i === indiceActual);
  });

  reiniciarCarrusel();
}

function moverFoto(direccion) {
  mostrarFoto(indiceActual + direccion);
}

function iniciarCarrusel() {
  temporizador = setInterval(() => moverFoto(1), 4500);
}

function reiniciarCarrusel() {
  clearInterval(temporizador);
  iniciarCarrusel();
}

function crearCorazon() {
  const corazon = document.createElement("div");
  const corazones = ["\u2665", "\ud83d\udc95", "\ud83d\udc97", "\ud83d\udc96"];
  corazon.className = "corazon";
  corazon.textContent = corazones[Math.floor(Math.random() * corazones.length)];
  corazon.style.left = Math.random() * 100 + "vw";
  corazon.style.fontSize = Math.random() * 18 + 16 + "px";
  corazon.style.animationDuration = Math.random() * 4 + 5 + "s";
  document.body.appendChild(corazon);

  setTimeout(() => corazon.remove(), 9500);
}

botonAnterior.addEventListener("click", () => moverFoto(-1));
botonSiguiente.addEventListener("click", () => moverFoto(1));

iniciarCarrusel();
setInterval(crearCorazon, 430);

const menuIcon = document.querySelector("#menu-bars");
const menu = document.querySelector(".menu");
const navbar = document.querySelector(".navbar");
const header = document.querySelector(".header");
function toggles() {
  menuIcon.classList.toggle("change");
  menu.classList.toggle("transformss");
}
function scrollUps(e) {
  let y = window.pageYOffset;
  let { height } = header.getBoundingClientRect();
  let { height: navheight } = navbar.getBoundingClientRect();
  let c = height - navheight;
  if (y >= c) {
    //make the position of navbar fixed
    navbar.classList.add("fixedposition");
  } else {
    navbar.classList.remove("fixedposition");
  }
}
menuIcon.addEventListener("click", toggles);
window.addEventListener("scroll", scrollUps);

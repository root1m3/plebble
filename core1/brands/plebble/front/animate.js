AOS.init({
  // Global settings:
  disable: true, // accepts following values: 'phone', 'tablet', 'mobile', boolean, expression or function
  throttleDelay: 99, // the delay on throttle used while scrolling the page (advanced)
  // Settings that can be overridden on per-element basis, by `data-aos-*` attributes:
  offset: 120, // offset (in px) from the original trigger point
  delay: 1000, // values from 0 to 3000, with step 50ms
  duration: 200, // values from 0 to 3000, with step 50ms
  easing: "ease", // default easing for AOS animations
  once: true, // whether animation should happen only once - while scrolling down
  mirror: false, // whether elements should animate out while scrolling past them
});
